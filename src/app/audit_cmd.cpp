#include "audit_cmd.h"
#include "../audit/audit.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult AuditCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    std::string query = args.size() > 1 ? args[1] : "";
    auto results = glia::audit::Audit::search(query);
    std::vector<std::string> headers = {Translator::t("col_date"), Translator::t("col_type"), Translator::t("col_repo"), Translator::t("col_snippet")};
    std::vector<std::vector<std::string>> rows;
    for (const auto& entry : results) {
        rows.push_back({entry.timestamp, entry.action, entry.target, entry.rationale});
    }
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
