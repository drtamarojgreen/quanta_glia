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

namespace fs = std::filesystem;
using glia::util::Translator;

namespace glia::app {

int countViolations(const RuleGlobals& globals, const std::vector<std::pair<std::string, std::regex>>& violations) {
    int totalViolations = 0;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (!entry.is_regular_file()) continue;

        bool ignored = false;
        for (const auto& idir : globals.ignoredDirs) {
            if (entry.path().string().find("/" + idir + "/") != std::string::npos ||
                entry.path().string().find("./" + idir + "/") == 0) {
                ignored = true;
                break;
            }
        }
        if (ignored) continue;

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
                totalViolations += count;
            }
        }
    }
    return totalViolations;
}

glia::core::CommandResult RestrictionsCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    std::vector<std::pair<std::string, std::regex>> violations;
    if (m_meta.lists.count("violations")) {
        for (const auto& v : m_meta.lists.at("violations")) {
            size_t sep = v.find('|');
            if (sep != std::string::npos) {
                violations.push_back({v.substr(0, sep), std::regex(v.substr(sep+1), std::regex::ECMAScript)});
            }
        }
    }

    int totalViolations = 0;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (!entry.is_regular_file()) continue;

        bool ignored = false;
        for (const auto& idir : globals.ignoredDirs) {
            if (entry.path().string().find("/" + idir + "/") != std::string::npos ||
                entry.path().string().find("./" + idir + "/") == 0) {
                ignored = true;
                break;
            }
        }
        if (ignored) continue;

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
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    auto allCommands = CommandLoader::loadFromXml("rules/rules.xml");

    // 1. Signal Calculation (LOC in src/ and include/)
    long long signal = 0;
    std::vector<std::string> scanDirs = {"src", "include"};
    for (const auto& dir : scanDirs) {
        if (!fs::exists(dir)) continue;
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;

            bool ignored = false;
            for (const auto& idir : globals.ignoredDirs) {
                if (entry.path().string().find("/" + idir + "/") != std::string::npos ||
                    entry.path().string().find("./" + idir + "/") == 0) {
                    ignored = true;
                    break;
                }
            }
            if (ignored) continue;

            std::string ext = entry.path().extension().string();
            bool validExt = false;
            for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
            if (!validExt) continue;

            std::ifstream file(entry.path());
            std::string line;
            while (std::getline(file, line)) signal++;
        }
    }

    // 2. Noise Calculation (Number of commands in rules.xml)
    double noise = static_cast<double>(allCommands.size());
    if (noise == 0) noise = 1.0; // Prevent div by zero

    // 3. Violations Calculation
    std::vector<std::pair<std::string, std::regex>> violationPatterns;
    // We try to find the 'restrictions' or 'verify-structure' command to get its patterns
    for (const auto& cmd : allCommands) {
        if (cmd.name == "restrictions" || cmd.name == "verify-structure") {
            if (cmd.lists.count("violations")) {
                for (const auto& v : cmd.lists.at("violations")) {
                    size_t sep = v.find('|');
                    if (sep != std::string::npos) {
                        violationPatterns.push_back({v.substr(0, sep), std::regex(v.substr(sep+1), std::regex::ECMAScript)});
                    }
                }
            }
            break;
        }
    }
    int violations = countViolations(globals, violationPatterns);

    // 4. Health Index Calculation
    double violation_threshold = 100.0;
    if (m_meta.params.count("violation_threshold")) {
        violation_threshold = std::stod(m_meta.params.at("violation_threshold"));
    }
    double multiplier = 1.0;
    if (m_meta.params.count("multiplier")) {
        multiplier = std::stod(m_meta.params.at("multiplier"));
    }

    double health_index = 1.0 - (static_cast<double>(violations) / violation_threshold) * multiplier;
    if (health_index < 0) health_index = 0;
    if (health_index > 1.0) health_index = 1.0;

    double snr = static_cast<double>(signal) / noise;
    double final_score = snr * health_index;

    std::cout << "SIGNAL = " << signal << std::endl;
    std::cout << "NOISE = " << noise << std::endl;
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
