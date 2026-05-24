#include "report_cmd.h"
#include "../report/reporter.h"
#include "../config/config.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include <iostream>
#include <filesystem>
namespace glia::app {
glia::core::CommandResult ReportCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    glia::config::Config config;
    config.load("config.yaml");

    std::string csvOut = m_meta.params.count("output_csv") ? m_meta.params.at("output_csv") : "repository_data.csv";

    std::vector<std::string> headers = {Translator::t("col_repo"), Translator::t("col_size"), Translator::t("col_src"), Translator::t("col_doc"), Translator::t("col_stack")};
    std::vector<std::vector<std::string>> rows;
    if (std::filesystem::exists(config.knowledgeBase)) {
        for (auto const& dir_entry : std::filesystem::directory_iterator(config.knowledgeBase)) {
            if (!dir_entry.is_directory()) continue;
            auto data = glia::report::Reporter::analyze(dir_entry.path().string());
            rows.push_back({data.repoName, std::to_string(data.repoSize), std::to_string(data.numSrcFiles), std::to_string(data.numDocFiles), data.techStack});
            glia::report::Reporter::writeCsv(data, csvOut);
        }
    }
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
