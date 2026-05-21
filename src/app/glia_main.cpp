#include "command.h"
#include "glia_git_cmds.h"
#include "glia_prune_cmd.h"
#include "glia_init_cmd.h"
#include "../cli/cli.h"
#include "lifecycle.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv) {
    glia::app::CommandRegistry registry;
    registry.registerCommand(std::make_unique<glia::app::CheckoutLatestCommand>());
    registry.registerCommand(std::make_unique<glia::app::CommitChangesCommand>());
    registry.registerCommand(std::make_unique<glia::app::UpdateRepoCommand>());
    registry.registerCommand(std::make_unique<glia::app::PruneCurrentCommand>());
    registry.registerCommand(std::make_unique<glia::app::GliaInitCommand>());

    glia::cli::ArgumentParser parser;
    parser.parse(argc, argv);

    std::vector<std::string> pos = parser.getPositional();
    std::string cmdName = pos.empty() ? "help" : pos[0];

    auto* cmd = registry.getCommand(cmdName);
    glia::core::CommandResult result;
    if (cmd) {
        result = cmd->execute(pos);
    } else if (cmdName == "help") {
        std::cout << "Glia - Developer Utility Tool\n";
        std::cout << "Available commands:\n";
        for (const auto& name : registry.listCommands()) {
            auto* c = registry.getCommand(name);
            std::cout << "  " << name << "\t" << c->description() << "\n";
        }
        result = {glia::core::ExitCode::Success, "Help displayed"};
    } else {
        std::cerr << "Unknown command: " << cmdName << "\n";
        std::cout << "Run 'glia help' for a list of commands.\n";
        result = {glia::core::ExitCode::UsageError, "Unknown command"};
    }

    if (!result.userMessage.empty()) {
        std::cout << (result.code == glia::core::ExitCode::Success ? "Success: " : "Error: ") 
                  << result.userMessage << std::endl;
    }

    return static_cast<int>(result.code);
}
