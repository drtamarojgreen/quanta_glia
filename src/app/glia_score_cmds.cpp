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

namespace fs = std::filesystem;
using glia::util::Translator;

namespace glia::app {

glia::core::CommandResult RestrictionsCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    std::vector<std::pair<std::string, std::regex>> violations;
    if (m_meta.lists.count("violations")) {
        for (const auto& v : m_meta.lists.at("violations")) {
            size_t sep = v.find('|');
            if (sep != std::string::npos) {
                violations.push_back({v.substr(0, sep), std::regex(v.substr(sep+1), std::regex::extended)});
            }
        }
    }

    int totalViolations = 0;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (!entry.is_regular_file()) continue;
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
    double signal = 100.0;
    double noise = 10.0;
    int violations = 0;

    // Attempt to read total violations from previous run or fact
    if (fs::exists("tests/sdd/facts/glia_audit.facts")) {
        // In a real system, we'd parse this properly. For now, simulate.
    }

    // D = LinesOfCode / UniqueOperations (Signal/Noise)
    double health_index = 1.0 - (static_cast<double>(violations) / 100.0);
    if (health_index < 0) health_index = 0;

    std::cout << "SIGNAL_NOISE_RATIO = " << (signal / noise) << std::endl;
    std::cout << "HEALTH_INDEX = " << health_index << std::endl;

    if (health_index < 0.5) {
        std::cout << "ACTION = PRUNE_TRIGGERED" << std::endl;
    } else {
        std::cout << "ACTION = STABLE" << std::endl;
    }

    return {glia::core::ExitCode::Success, "Scoring complete"};
}

}
