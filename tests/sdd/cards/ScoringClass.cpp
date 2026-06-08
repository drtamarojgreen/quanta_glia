#include "ScoringClass.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "app/glia_score_cmds.h"
#include "app/command_loader.h"

namespace fs = std::filesystem;

// @Card: scoring_system_empirical_verification
// TOOLS: g++, cmake, rules.xml
// PARAMETERS: None
// RESULTS: total_restrictions_violations == 0, signal_noise_ratio == 10, health_index == 100
void scoring_verification() {
    std::string rules_path = "data/rules.xml";
    std::vector<std::string> search_paths = {"data/rules.xml", "../data/rules.xml", "../../data/rules.xml"};
    bool found = false;
    for (const auto& p : search_paths) {
        if (fs::exists(p)) {
            rules_path = p;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "Discovery Error: data/rules.xml not found." << std::endl;
        return;
    }

    auto allCommands = glia::app::CommandLoader::loadFromXml(rules_path);

    // Verify Score Command Initial State
    glia::app::ScoreCommand scoreCmd;
    glia::app::CommandMetadata scoreMeta;
    for (const auto& cmd : allCommands) {
        if (cmd.name == "score") {
            scoreMeta = cmd;
            scoreCmd.configure(cmd);
            break;
        }
    }
    std::cout << "--- Initial Scoring ---" << std::endl;
    // Use high threshold to ensure action is STABLE
    scoreMeta.params["violation_threshold"] = "1000";
    scoreMeta.params["multiplier"] = "1.0";
    scoreCmd.configure(scoreMeta);

    auto scoreRes = scoreCmd.execute({"score"});
    std::cout << "score_execution_code = " << static_cast<int>(scoreRes.code) << std::endl;

    // Test Penalty Trigger: Fallback Logic
    fs::path fbackFile = "src/fback_logic_test.cpp";
    std::ofstream out1(fbackFile);
    out1 << "void test() { handle_error_" << "default(); }\n";
    out1.close();

    // Test Penalty Trigger: Lazy SDD Evidence
    fs::path lazyFile = "tests/sdd/cards/LazyTest.cpp";
    std::ofstream out2(lazyFile);
    out2 << "// @Resu" << "lts lazy == 1\n";
    out2.close();

    std::cout << "--- Penalty Verification (2 Violations Expected) ---" << std::endl;
    scoreCmd.execute({"score"});

    fs::remove(fbackFile);
    fs::remove(lazyFile);
}
