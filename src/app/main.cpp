#include "config_cmd.h"
#include "audit_cmd.h"
#include <chrono>
#include "trigger/trigger.h"
#include "annotate_cmd.h"
#include "status_cmd.h"
#include <iostream>
#include <vector>
#include <string>
#include "command.h"
#include "lifecycle.h"
#include "about.h"
#include "init.h"
#include "capabilities.h"
#include "harvest_cmd.h"
#include "prune_cmd.h"
#include "report_cmd.h"
#include "../config/config.h"
#include "../cli/cli.h"

int main(int argc, char** argv) {
    glia::app::Lifecycle::preRun();

    glia::app::CommandRegistry registry;
    registry.registerCommand(std::make_unique<glia::app::AboutCommand>());
    registry.registerCommand(std::make_unique<glia::app::InitCommand>());
    registry.registerCommand(std::make_unique<glia::app::CapabilitiesCommand>());
    registry.registerCommand(std::make_unique<glia::app::HarvestCommand>());
    registry.registerCommand(std::make_unique<glia::app::PruneCommand>());
    registry.registerCommand(std::make_unique<glia::app::ReportCommand>());
    registry.registerCommand(std::make_unique<glia::app::StatusCommand>());
    registry.registerCommand(std::make_unique<glia::app::AnnotateCommand>());
    registry.registerCommand(std::make_unique<glia::app::AuditCommand>());
    registry.registerCommand(std::make_unique<glia::app::ConfigCommand>());

    for (const auto& name : registry.listCommands()) {
        registry.getCommand(name)->setRegistry(&registry);
    }

    glia::cli::ArgumentParser parser;
    parser.parse(argc, argv);

    std::vector<std::string> pos = parser.getPositional();
    std::string cmdName = pos.empty() ? "status" : pos[0];

    auto startTime = std::chrono::steady_clock::now();
    auto* cmd = registry.getCommand(cmdName);
    glia::core::CommandResult result;
    if (cmd) {
        result = cmd->execute(pos);
    } else if (cmdName == "help") {
        std::cout << "Available commands:\n";
        for (const auto& name : registry.listCommands()) {
            auto* c = registry.getCommand(name);
            std::cout << "  " << name << "\t" << c->description() << "\n";
        }
        result = {glia::core::ExitCode::Success, "Help displayed"};
    } else {
        result = {glia::core::ExitCode::UsageError, "Unknown command: " + cmdName};
    }

    glia::app::Lifecycle::postRun(result);
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    glia::trigger::TriggerEngine::record({cmdName, static_cast<int>(result.code), duration});
    return static_cast<int>(result.code);
}
