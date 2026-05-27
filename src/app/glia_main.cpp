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
#include "glia_qtl_cmd.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include "../core/state.h"
#include "../config/config.h"
#include "lifecycle.h"
#include <iostream>
#include <vector>
#include <string>

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
    reg("QtlCommand", [](){ return std::make_unique<QtlCommand>(); });
}

int main(int argc, char** argv) {
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

    for (const auto& name : registry.listCommands()) {
        registry.getCommand(name)->setRegistry(&registry);
    }

    glia::cli::ArgumentParser parser;
    parser.parse(argc, argv);
    std::vector<std::string> pos = parser.getPositional();
    std::string cmdName = pos.empty() ? "status" : pos[0];

    auto* cmd = registry.getCommand(cmdName);
    glia::core::CommandResult result;
    if (cmd) {
        State::set("last_cmd", cmdName);
        result = cmd->execute(pos);
        State::save(appCfg.statePath);
    } else if (cmdName == "help") {
        std::cout << Translator::t("help_header") << "\n" << Translator::t("help_available") << "\n";
        for (const auto& name : registry.listCommands()) {
            auto* c = registry.getCommand(name);
            std::cout << "  " << name << "\t" << c->description() << "\n";
        }
        result = {glia::core::ExitCode::Success, Translator::t("msg_done")};
    } else {
        std::cerr << Translator::t("msg_error") << ": " << cmdName << "\n";
        result = {glia::core::ExitCode::UsageError, ""};
    }

    if (!result.userMessage.empty()) {
        bool isSuccess = (result.code == glia::core::ExitCode::Success || result.code == glia::core::ExitCode::PartialSuccess);
        std::cout << (isSuccess ? Translator::t("msg_success") : Translator::t("msg_error"))
                  << ": " << result.userMessage << std::endl;
    }
    for (const auto& hint : result.hints) std::cout << "  * " << hint << std::endl;

    return static_cast<int>(result.code);
}
