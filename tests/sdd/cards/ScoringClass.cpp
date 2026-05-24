#include "ScoringClass.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include "app/glia_score_cmds.h"
#include "app/command_loader.h"

namespace fs = std::filesystem;

void scoring_verification() {
    std::string rules_path = "rules/rules.xml";
    if (!fs::exists(rules_path)) {
        std::cerr << "Discovery Error: " << rules_path << " not found." << std::endl;
        return;
    }

    auto allCommands = glia::app::CommandLoader::loadFromXml(rules_path);

    // Verify Restrictions
    glia::app::RestrictionsCommand restCmd;
    for (const auto& cmd : allCommands) {
        if (cmd.name == "restrictions") {
            restCmd.configure(cmd);
            break;
        }
    }
    auto restRes = restCmd.execute({"restrictions"});
    std::cout << "restrictions_execution_code = " << static_cast<int>(restRes.code) << std::endl;

    // Verify Score
    glia::app::ScoreCommand scoreCmd;
    for (const auto& cmd : allCommands) {
        if (cmd.name == "score") {
            scoreCmd.configure(cmd);
            break;
        }
    }
    auto scoreRes = scoreCmd.execute({"score"});
    std::cout << "score_execution_code = " << static_cast<int>(scoreRes.code) << std::endl;
}
