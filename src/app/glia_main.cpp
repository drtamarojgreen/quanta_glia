#include "command.h"
#include "command_loader.h"
#include "command_factory.h"
#include "glia_git_cmds.h"
#include "glia_workspace_cmds.h"
#include "glia_waste_cmds.h"
#include "glia_prune_cmd.h"
#include "glia_init_cmd.h"
#include "harvest_cmd.h"
#include "report_cmd.h"
#include "status_cmd.h"
#include "audit_cmd.h"
#include "prune_cmd.h"
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

using namespace glia::app;

void registerAllCreators() {
    CommandFactory::registerCreator("CheckoutLatestCommand", [](){ return std::make_unique<CheckoutLatestCommand>(); });
    CommandFactory::registerCreator("CommitChangesCommand", [](){ return std::make_unique<CommitChangesCommand>(); });
    CommandFactory::registerCreator("UpdateRepoCommand", [](){ return std::make_unique<UpdateRepoCommand>(); });
    CommandFactory::registerCreator("QuickCommitCommand", [](){ return std::make_unique<QuickCommitCommand>(); });
    CommandFactory::registerCreator("WorkspaceStatusCommand", [](){ return std::make_unique<WorkspaceStatusCommand>(); });
    CommandFactory::registerCreator("WorkspaceSyncCommand", [](){ return std::make_unique<WorkspaceSyncCommand>(); });
    CommandFactory::registerCreator("WasteScanCommand", [](){ return std::make_unique<WasteScanCommand>(); });
    CommandFactory::registerCreator("VerifyStructureCommand", [](){ return std::make_unique<VerifyStructureCommand>(); });
    CommandFactory::registerCreator("AuditRepetitionCommand", [](){ return std::make_unique<AuditRepetitionCommand>(); });
    CommandFactory::registerCreator("AuditCommitsCommand", [](){ return std::make_unique<AuditCommitsCommand>(); });
    CommandFactory::registerCreator("HarvestCommand", [](){ return std::make_unique<HarvestCommand>(); });
    CommandFactory::registerCreator("ReportCommand", [](){ return std::make_unique<ReportCommand>(); });
    CommandFactory::registerCreator("StatusCommand", [](){ return std::make_unique<StatusCommand>(); });
    CommandFactory::registerCreator("AuditCommand", [](){ return std::make_unique<AuditCommand>(); });
    CommandFactory::registerCreator("PruneCommand", [](){ return std::make_unique<PruneCommand>(); });
    CommandFactory::registerCreator("PruneCurrentCommand", [](){ return std::make_unique<PruneCurrentCommand>(); });
    CommandFactory::registerCreator("GliaInitCommand", [](){ return std::make_unique<GliaInitCommand>(); });
    CommandFactory::registerCreator("ConfigCommand", [](){ return std::make_unique<ConfigCommand>(); });
    CommandFactory::registerCreator("AnnotateCommand", [](){ return std::make_unique<AnnotateCommand>(); });
    CommandFactory::registerCreator("AboutCommand", [](){ return std::make_unique<AboutCommand>(); });
    CommandFactory::registerCreator("InitCommand", [](){ return std::make_unique<InitCommand>(); });
    CommandFactory::registerCreator("CapabilitiesCommand", [](){ return std::make_unique<CapabilitiesCommand>(); });
}

int main(int argc, char** argv) {
    registerAllCreators();

    CommandRegistry registry;
    auto metaList = CommandLoader::loadFromXml("rules/rules.xml");
    for (const auto& meta : metaList) {
        auto cmd = CommandFactory::create(meta);
        if (cmd) registry.registerCommand(std::move(cmd));
    }

    glia::cli::ArgumentParser parser;
    parser.parse(argc, argv);

    std::vector<std::string> pos = parser.getPositional();
    std::string cmdName = pos.empty() ? "help" : pos[0];

    auto* cmd = registry.getCommand(cmdName);
    glia::core::CommandResult result;
    if (cmd) {
        result = cmd->execute(pos);
    } else if (cmdName == "help") {
        std::cout << "Glia - Developer Utility Tool (Modular XML Edition)\n";
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
