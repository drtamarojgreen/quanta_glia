#include "ScoringClass.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "app/glia_score_cmds.h"
#include "app/command_loader.h"

namespace fs = std::filesystem;

void scoring_verification() {
    glia::app::ScoreCommand scoreCmd;

    auto allCommands = glia::app::CommandLoader::loadFromXml("rules/rules.xml");
    for (const auto& cmd : allCommands) {
        if (cmd.name == "score") {
            scoreCmd.configure(cmd);
            break;
        }
    }

    auto res = scoreCmd.execute({"score"});
    std::cout << "scoring_execution_code = " << static_cast<int>(res.code) << std::endl;
}
