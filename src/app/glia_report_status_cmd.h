#ifndef GLIA_APP_REPORT_STATUS_CMD_H
#define GLIA_APP_REPORT_STATUS_CMD_H

#include "command.h"
#include "../util/shell_utils.h"
#include <iostream>
#include <vector>

namespace glia::app {

class ReportStatusCommand : public Command {
public:
    ReportStatusCommand(const CommandRegistry& registry) : m_registry(registry) {}

    std::string name() const override { return "report-status"; }
    std::string description() const override { return "Report test status and available commands"; }

    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        std::cout << "--- Glia System Status Report ---\n\n";

        // 1. Dynamic Command Availability
        std::cout << "[Available Commands]\n";
        auto commands = m_registry.listCommands();
        for (const auto& cmdName : commands) {
            auto* cmd = m_registry.getCommand(cmdName);
            if (cmd) {
                std::cout << "  - " << cmdName << ": " << cmd->description() << "\n";
            }
        }
        std::cout << "\n";

        // 2. Real Test Execution
        std::cout << "[Test Suites]\n";
        std::cout << "  Running glia_unit_tests...\n";
        int testResult = glia::util::Shell::run("./glia_unit_tests", true);
        if (testResult == 0) {
            std::cout << "  - Unit Tests: PASSED\n";
        } else {
            std::cout << "  - Unit Tests: FAILED (Exit code: " << testResult << ")\n";
        }

        return {glia::core::ExitCode::Success, "Status report generated successfully"};
    }

private:
    const CommandRegistry& m_registry;
};

}

#endif
