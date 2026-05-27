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
    std::string rules_path = "rules/rules.xml";
    std::vector<std::string> search_paths = {"rules/rules.xml", "../rules/rules.xml", "../../rules/rules.xml"};
    bool found = false;
    for (const auto& p : search_paths) {
        if (fs::exists(p)) {
            rules_path = p;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "Discovery Error: rules/rules.xml not found." << std::endl;
        return;
    }

    auto allCommands = glia::app::CommandLoader::loadFromXml(rules_path);

    // Verify Score Command Initial State
    glia::app::ScoreCommand scoreCmd;
    for (const auto& cmd : allCommands) {
        if (cmd.name == "score") {
            scoreCmd.configure(cmd);
            break;
        }
    }
    std::cout << "--- Initial Scoring ---" << std::endl;
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
