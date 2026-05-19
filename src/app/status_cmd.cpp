#include "status_cmd.h"
#include "../cli/cli.h"
#include <iostream>
#include <filesystem>
namespace glia::app {
glia::core::CommandResult StatusCommand::execute(const std::vector<std::string>& args) {
    std::vector<std::string> headers = {"Module", "Status", "Records"};
    std::vector<std::vector<std::string>> rows = {
        {"Knowledge Base", "Active", "0"},
        {"Audit Log", "Enabled", "N/A"},
        {"Pruner", "Conservative", "0"}
    };
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Status displayed"};
}
}
