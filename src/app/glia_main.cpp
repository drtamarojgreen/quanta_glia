#include "command.h"
#include "glia_git_cmds.h"
#include "glia_workspace_cmds.h"
#include "glia_waste_cmds.h"
#include "glia_prune_cmd.h"
#include "glia_init_cmd.h"
#include "prune_cmd.h"
#include "harvest_cmd.h"
#include "report_cmd.h"
#include "status_cmd.h"
#include "audit_cmd.h"
#include "config_cmd.h"
#include "annotate_cmd.h"
#include "about.h"
#include "init.h"
#include "capabilities.h"
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
    registry.registerCommand(std::make_unique<glia::app::QuickCommitCommand>());
    registry.registerCommand(std::make_unique<glia::app::WorkspaceStatusCommand>());
    registry.registerCommand(std::make_unique<glia::app::WorkspaceSyncCommand>());
    registry.registerCommand(std::make_unique<glia::app::WasteScanCommand>());
    registry.registerCommand(std::make_unique<glia::app::VerifyStructureCommand>());
    registry.registerCommand(std::make_unique<glia::app::AuditRepetitionCommand>());
    registry.registerCommand(std::make_unique<glia::app::PruneCurrentCommand>());
    registry.registerCommand(std::make_unique<glia::app::GliaInitCommand>());

    // Core functionality from main app
    registry.registerCommand(std::make_unique<glia::app::PruneCommand>());
    registry.registerCommand(std::make_unique<glia::app::HarvestCommand>());
    registry.registerCommand(std::make_unique<glia::app::ReportCommand>());
    registry.registerCommand(std::make_unique<glia::app::StatusCommand>());
    registry.registerCommand(std::make_unique<glia::app::AuditCommand>());
    registry.registerCommand(std::make_unique<glia::app::ConfigCommand>());
    registry.registerCommand(std::make_unique<glia::app::AnnotateCommand>());
    registry.registerCommand(std::make_unique<glia::app::AboutCommand>());
    registry.registerCommand(std::make_unique<glia::app::InitCommand>());
    registry.registerCommand(std::make_unique<glia::app::CapabilitiesCommand>());

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
        bool isSuccess = (result.code == glia::core::ExitCode::Success || result.code == glia::core::ExitCode::PartialSuccess);
        std::cout << (isSuccess ? "Success: " : "Error: ")
                  << result.userMessage << std::endl;
    }

    for (const auto& hint : result.hints) {
        std::cout << "  * Hint: " << hint << std::endl;
    }

    return static_cast<int>(result.code);
}
