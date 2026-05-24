#include "status_cmd.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include <iostream>
#include <vector>

namespace glia::app {
glia::core::CommandResult StatusCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    std::vector<std::string> headers = {Translator::t("col_module"), Translator::t("col_status")};
    std::vector<std::vector<std::string>> rows = {
        {"KB", Translator::t("msg_ok")},
        {"Audit", Translator::t("msg_ok")}
    };
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, ""};
}
}
