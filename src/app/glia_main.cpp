#include "command.h"
#include "command_loader.h"
#include "command_factory.h"
#include "glia_git_cmds.h"
#include "glia_workspace_cmds.h"
#include "glia_waste_cmds.h"
#include "glia_discovery_cmds.h"
#include "glia_score_cmds.h"
#include "glia_prune_cmd.h"
#include "glia_init_cmd.h"
<<<<<<< HEAD
#include "glia_tui_cmd.h"
#include "glia_report_status_cmd.h"
#include "rule_loader.h"
=======
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
#include "genome_cmd.h"
>>>>>>> origin/glia-workflow-enhancement-15384845863160463952
#include "../cli/cli.h"
#include "../util/translator.h"
#include "../core/state.h"
#include "../config/config.h"
#include "lifecycle.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace glia::app;

using namespace glia::app;
using glia::util::Translator;
using glia::core::State;
using glia::config::Config;

void registerAllCreators() {
    auto reg = [](const std::string& target, auto creator) {
        CommandFactory::registerCreator(target, [creator](){ return creator(); });
    };

    reg("CheckoutLatestCommand", [](){ return std::make_unique<CheckoutLatestCommand>(); });
    reg("CommitChangesCommand", [](){ return std::make_unique<CommitChangesCommand>(); });
    reg("UpdateRepoCommand", [](){ return std::make_unique<UpdateRepoCommand>(); });
    reg("QuickCommitCommand", [](){ return std::make_unique<QuickCommitCommand>(); });
    reg("WorkspaceStatusCommand", [](){ return std::make_unique<WorkspaceStatusCommand>(); });
    reg("WorkspaceSyncCommand", [](){ return std::make_unique<WorkspaceSyncCommand>(); });
    reg("WorkspaceMeshCommand", [](){ return std::make_unique<WorkspaceMeshCommand>(); });
    reg("WasteScanCommand", [](){ return std::make_unique<WasteScanCommand>(); });
    reg("VerifyStructureCommand", [](){ return std::make_unique<VerifyStructureCommand>(); });
    reg("AuditRepetitionCommand", [](){ return std::make_unique<AuditRepetitionCommand>(); });
    reg("AuditCommitsCommand", [](){ return std::make_unique<AuditCommitsCommand>(); });
    reg("HarvestCommand", [](){ return std::make_unique<HarvestCommand>(); });
    reg("ReportCommand", [](){ return std::make_unique<ReportCommand>(); });
    reg("StatusCommand", [](){ return std::make_unique<StatusCommand>(); });
    reg("AuditCommand", [](){ return std::make_unique<AuditCommand>(); });
    reg("PruneCommand", [](){ return std::make_unique<PruneCommand>(); });
    reg("PruneCurrentCommand", [](){ return std::make_unique<PruneCurrentCommand>(); });
    reg("GliaInitCommand", [](){ return std::make_unique<GliaInitCommand>(); });
    reg("ConfigCommand", [](){ return std::make_unique<ConfigCommand>(); });
    reg("AnnotateCommand", [](){ return std::make_unique<AnnotateCommand>(); });
    reg("AboutCommand", [](){ return std::make_unique<AboutCommand>(); });
    reg("InitCommand", [](){ return std::make_unique<InitCommand>(); });
    reg("CapabilitiesCommand", [](){ return std::make_unique<CapabilitiesCommand>(); });
    reg("GenomeCommand", [](){ return std::make_unique<GenomeCommand>(); });
    reg("DiscoverCommand", [](){ return std::make_unique<DiscoverCommand>(); });
    reg("GateCheckCommand", [](){ return std::make_unique<GateCheckCommand>(); });
    reg("RestrictionsCommand", [](){ return std::make_unique<RestrictionsCommand>(); });
    reg("ScoreCommand", [](){ return std::make_unique<ScoreCommand>(); });
}

int main(int argc, char** argv) {
<<<<<<< HEAD
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
=======
    registerAllCreators();

    Config appCfg;
    appCfg.load("config.yaml");

    CommandRegistry registry;
    auto metaList = CommandLoader::loadFromXml(appCfg.rulesPath);
    auto globals = CommandLoader::loadGlobals(appCfg.rulesPath);
    Translator::load(globals.uiStrings);
    State::load(appCfg.statePath);

    for (const auto& meta : metaList) {
        auto cmd = CommandFactory::create(meta);
        if (cmd) {
            cmd->configure(meta);
            registry.registerCommand(std::move(cmd));
        }
    }
>>>>>>> origin/glia-workflow-enhancement-15384845863160463952

    glia::cli::ArgumentParser parser;
    parser.addOption("--version", "-v", "Display version number");
    parser.addOption("--update", "-u", "Update glia to specific version");
    parser.parse(argc, argv);
<<<<<<< HEAD

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

=======
>>>>>>> origin/glia-workflow-enhancement-15384845863160463952
    std::vector<std::string> pos = parser.getPositional();
    std::string cmdName = pos.empty() ? "help" : pos[0];

    auto* cmd = registry.getCommand(cmdName);
    glia::core::CommandResult result;
    if (cmd) {
        State::set("last_cmd", cmdName);
        result = cmd->execute(pos);
        State::save(appCfg.statePath);
    } else if (cmdName == "help") {
<<<<<<< HEAD
        std::cout << "====================================================\n";
        std::cout << " GLIA - Developer Automation & Knowledge Engine     \n";
        std::cout << " Version: 1.1.0 (Standalone Production)             \n";
        std::cout << "====================================================\n\n";
        
        std::cout << "Glia is a data-driven automation platform. Commands \n";
        std::cout << "are loaded dynamically from ~/.glia/rules.xml.      \n\n";

        std::cout << "Usage: glia <command> [args]\n";
        std::cout << "       glia [options]\n\n";

        std::cout << "Available Commands:\n";
=======
        std::cout << Translator::t("help_header") << "\n" << Translator::t("help_available") << "\n";
>>>>>>> origin/glia-workflow-enhancement-15384845863160463952
        for (const auto& name : registry.listCommands()) {
            auto* c = registry.getCommand(name);
            std::cout << "  " << std::left << std::setw(20) << name << c->description() << "\n";
        }
<<<<<<< HEAD
        
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
=======
        result = {glia::core::ExitCode::Success, Translator::t("msg_done")};
    } else {
        std::cerr << Translator::t("msg_error") << ": " << cmdName << "\n";
        result = {glia::core::ExitCode::UsageError, ""};
>>>>>>> origin/glia-workflow-enhancement-15384845863160463952
    }

    if (!result.userMessage.empty()) {
        bool isSuccess = (result.code == glia::core::ExitCode::Success || result.code == glia::core::ExitCode::PartialSuccess);
        std::cout << (isSuccess ? Translator::t("msg_success") : Translator::t("msg_error"))
                  << ": " << result.userMessage << std::endl;
    }
    for (const auto& hint : result.hints) std::cout << "  * " << hint << std::endl;

    return static_cast<int>(result.code);
}
