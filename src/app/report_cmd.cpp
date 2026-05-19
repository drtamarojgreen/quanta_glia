#include "report_cmd.h"
#include "../report/reporter.h"
#include "../config/config.h"
#include "../cli/cli.h"
#include <iostream>
#include <filesystem>
namespace glia::app {
glia::core::CommandResult ReportCommand::execute(const std::vector<std::string>& args) {
    glia::config::Config config;
    config.load("config.yaml");
    std::cout << "Generating Report...\n";
    std::vector<std::string> headers = {"Repository", "Size", "Src", "Doc", "Stack"};
    std::vector<std::vector<std::string>> rows;
    if (std::filesystem::exists(config.knowledgeBase)) {
        for (auto const& dir_entry : std::filesystem::directory_iterator(config.knowledgeBase)) {
            if (!dir_entry.is_directory()) continue;
            auto data = glia::report::Reporter::analyze(dir_entry.path().string());
            rows.push_back({data.repoName, std::to_string(data.repoSize), std::to_string(data.numSrcFiles), std::to_string(data.numDocFiles), data.techStack});
            glia::report::Reporter::writeCsv(data, "repository_data.csv");
        }
    }
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Report complete"};
}
}
