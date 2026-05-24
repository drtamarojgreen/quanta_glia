#include "glia_score_cmds.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include "command_loader.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <iterator>

namespace fs = std::filesystem;
using glia::util::Translator;

namespace glia::app {

namespace {
    bool shouldIgnore(const fs::path& path, const std::vector<std::string>& ignored) {
        for (const auto& part : path) {
            for (const auto& ig : ignored) {
                if (part == ig) return true;
            }
        }
        return false;
    }
}

glia::core::CommandResult RestrictionsCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    std::vector<std::pair<std::string, std::regex>> violations;
    if (m_meta.lists.count("violations")) {
        for (const auto& v : m_meta.lists.at("violations")) {
            size_t sep = v.find('|');
            if (sep != std::string::npos) {
                violations.push_back({v.substr(0, sep), std::regex(v.substr(sep+1))});
            }
        }
    }

    int totalViolations = 0;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (!entry.is_regular_file()) continue;
        if (shouldIgnore(entry.path(), globals.ignoredDirs)) continue;

        std::string ext = entry.path().extension().string();
        bool validExt = false;
        for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
        if (!validExt) continue;

        std::ifstream file(entry.path());
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        for (const auto& [name, regex] : violations) {
            auto v_begin = std::sregex_iterator(content.begin(), content.end(), regex);
            auto v_end = std::sregex_iterator();
            int count = std::distance(v_begin, v_end);
            if (count > 0) {
                std::cout << "RESTRICTION_VIOLATION [" << name << "] in " << entry.path().filename().string() << " count = " << count << std::endl;
                totalViolations += count;
            }
        }
    }

    std::cout << "total_restrictions_violations = " << totalViolations << std::endl;
    return {totalViolations == 0 ? glia::core::ExitCode::Success : glia::core::ExitCode::InternalFailure,
            totalViolations == 0 ? "Restrictions met" : "Structural violations detected"};
}

glia::core::CommandResult ScoreCommand::execute(const std::vector<std::string>& args) {
    auto allCmds = CommandLoader::loadFromXml("rules/rules.xml");
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");

    // 1. Calculate Signal (LOC)
    long totalLOC = 0;
    for (const auto& dir : {"src", "include"}) {
        if (!fs::exists(dir)) continue;
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
            if (shouldIgnore(entry.path(), globals.ignoredDirs)) continue;
            std::ifstream file(entry.path());
            totalLOC += std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
        }
    }
    double signal = static_cast<double>(totalLOC);

    // 2. Calculate Noise (Command Count)
    double noise = static_cast<double>(allCmds.size());
    if (noise == 0) noise = 1.0;

    // 3. Calculate Structural Violations
    int violations = 0;
    auto restIt = std::find_if(allCmds.begin(), allCmds.end(), [](const auto& m) { return m.name == "restrictions"; });
    if (restIt != allCmds.end() && restIt->lists.count("violations")) {
        std::vector<std::pair<std::string, std::regex>> patterns;
        for (const auto& v : restIt->lists.at("violations")) {
            size_t sep = v.find('|');
            if (sep != std::string::npos) {
                patterns.push_back({v.substr(0, sep), std::regex(v.substr(sep+1))});
            }
        }

        for (const auto& dir : {"src", "include"}) {
            if (!fs::exists(dir)) continue;
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (!entry.is_regular_file()) continue;
                if (shouldIgnore(entry.path(), globals.ignoredDirs)) continue;

                std::string ext = entry.path().extension().string();
                bool validExt = false;
                for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
                if (!validExt) continue;

                std::ifstream file(entry.path());
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                for (const auto& [name, regex] : patterns) {
                    auto v_begin = std::sregex_iterator(content.begin(), content.end(), regex);
                    auto v_end = std::sregex_iterator();
                    violations += std::distance(v_begin, v_end);
                }
            }
        }
    }

    // 4. Calculate Health Index
    double threshold = 100.0;
    if (m_meta.params.count("violation_threshold")) {
        threshold = std::stod(m_meta.params.at("violation_threshold"));
    }
    double health_index = 1.0 - (static_cast<double>(violations) / threshold);
    if (health_index < 0) health_index = 0;

    // 5. Final Score Calculation
    double multiplier = 1.0;
    if (m_meta.params.count("multiplier")) {
        multiplier = std::stod(m_meta.params.at("multiplier"));
    }
    double snr = signal / noise;
    double final_score = snr * health_index * multiplier;

    std::cout << "SIGNAL (LOC) = " << signal << std::endl;
    std::cout << "NOISE (CMDS) = " << noise << std::endl;
    std::cout << "SIGNAL_NOISE_RATIO = " << snr << std::endl;
    std::cout << "VIOLATIONS = " << violations << std::endl;
    std::cout << "HEALTH_INDEX = " << health_index << std::endl;
    std::cout << "FINAL_SCORE = " << final_score << std::endl;

    if (health_index < 0.5) {
        std::cout << "ACTION = PRUNE_TRIGGERED" << std::endl;
    } else {
        std::cout << "ACTION = STABLE" << std::endl;
    }

    return {glia::core::ExitCode::Success, "Scoring complete"};
}

}
