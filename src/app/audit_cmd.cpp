#include "audit_cmd.h"
#include "../audit/audit.h"
#include "../cli/cli.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult AuditCommand::execute(const std::vector<std::string>& args) {
    std::string query = args.size() > 1 ? args[1] : "";
    auto results = glia::audit::Audit::search(query);
    std::vector<std::string> headers = {"Timestamp", "Action", "Target", "Rationale"};
    std::vector<std::vector<std::string>> rows;
    for (const auto& entry : results) {
        rows.push_back({entry.timestamp, entry.action, entry.target, entry.rationale});
    }
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Audit search complete"};
}
}
