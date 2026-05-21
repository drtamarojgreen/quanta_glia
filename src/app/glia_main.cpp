#include "command.h"
#include "glia_git_cmds.h"
#include "glia_prune_cmd.h"
#include "glia_init_cmd.h"
#include "glia_tui_cmd.h"
#include "glia_report_status_cmd.h"
#include "rule_loader.h"
#include "../cli/cli.h"
#include "lifecycle.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace glia::app;

int main(int argc, char** argv) {
    CommandRegistry registry;

    // --- Core Commands (Hardcoded for bootstrap/safety) ---
    registry.registerCommand(std::make_unique<CheckoutLatestCommand>());
    registry.registerCommand(std::make_unique<CommitChangesCommand>());
    registry.registerCommand(std::make_unique<UpdateRepoCommand>());
    registry.registerCommand(std::make_unique<PruneCurrentCommand>());
    registry.registerCommand(std::make_unique<GliaInitCommand>());
    registry.registerCommand(std::make_unique<TuiCommand>());
    registry.registerCommand(std::make_unique<ReportStatusCommand>(registry));

    // --- Dynamic Commands (Loaded from global ~/.glia) ---
    char* home = std::getenv("HOME");
    std::string rulesPath;
    if (home) {
        rulesPath = std::string(home) + "/.glia/rules.xml";
    }

    if (rulesPath.empty() || !std::filesystem::exists(rulesPath)) {
        // Fallback for development if not initialized
        rulesPath = "data/rules.xml";
        if (!std::filesystem::exists(rulesPath)) {
            rulesPath = "../data/rules.xml";
        }
    }
    
    if (std::filesystem::exists(rulesPath)) {
        RuleLoader::load(rulesPath, registry);
    }

    glia::cli::ArgumentParser parser;
    parser.addOption("--version", "-v", "Display version number");
    parser.addOption("--update", "-u", "Update glia to specific version");
    parser.parse(argc, argv);

    if (parser.hasFlag("--version")) {
        std::cout << "glia version 1.1.0\n";
        return 0;
    }

    if (parser.hasFlag("--update")) {
        std::string target = parser.getOption("--update");
        if (target.empty()) {
            std::cerr << "Error: --update requires a version number\n";
            return 1;
        }
        std::cout << "Updating glia to version " << target << "...\n";
        
        // Real Git update logic
        int fetchCode = glia::util::Shell::run("git fetch --tags", true);
        if (fetchCode != 0) {
            std::cerr << "Error: Failed to fetch tags from repository.\n";
            return 1;
        }

        std::string checkoutCmd = "git checkout " + target;
        int checkoutCode = glia::util::Shell::run(checkoutCmd, true);
        if (checkoutCode == 0) {
            std::cout << "Successfully updated to " << target << ". Please rebuild the tool.\n";
            return 0;
        } else {
            std::cerr << "Error: Version " << target << " not found or checkout failed.\n";
            return 1;
        }
    }

    std::vector<std::string> pos = parser.getPositional();
    std::string cmdName = pos.empty() ? "help" : pos[0];

    auto* cmd = registry.getCommand(cmdName);
    glia::core::CommandResult result;
    if (cmd) {
        result = cmd->execute(pos);
    } else if (cmdName == "help") {
        std::cout << "====================================================\n";
        std::cout << " GLIA - Developer Automation & Knowledge Engine     \n";
        std::cout << " Version: 1.1.0 (Standalone Production)             \n";
        std::cout << "====================================================\n\n";
        
        std::cout << "Glia is a data-driven automation platform. Commands \n";
        std::cout << "are loaded dynamically from ~/.glia/rules.xml.      \n\n";

        std::cout << "Usage: glia <command> [args]\n";
        std::cout << "       glia [options]\n\n";

        std::cout << "Available Commands:\n";
        for (const auto& name : registry.listCommands()) {
            auto* c = registry.getCommand(name);
            std::cout << "  " << std::left << std::setw(20) << name << c->description() << "\n";
        }
        
        std::cout << "\nAutomation Engine Features:\n";
        std::cout << "  - Sequences: Multi-step workflows with failure protection.\n";
        std::cout << "  - TUI: Interactive node-graph editor (run 'glia tui').\n";
        std::cout << "  - Global: Persistent config stored in ~/.glia/\n";

        std::cout << "\nOptions:\n";
        std::cout << "  -v, --version       Display version number\n";
        std::cout << "  -u, --update <v>    Perform real Git-based update to tag\n";
        std::cout << "  help                Display this comprehensive help\n\n";
        
        std::cout << "Configuration: ~/.glia/rules.xml (validated by rules.xsd)\n";
        std::cout << "====================================================\n";
        result = {glia::core::ExitCode::Success, "Help displayed"};
    } else {
        std::cerr << "Unknown command: " << cmdName << "\n";
        result = {glia::core::ExitCode::UsageError, "Unknown command"};
    }

    if (!result.userMessage.empty()) {
        std::cout << (result.code == glia::core::ExitCode::Success ? "Success: " : "Error: ") 
                  << result.userMessage << std::endl;
    }

    return static_cast<int>(result.code);
}
