#include "glia_waste_cmds.h"
#include "../cli/cli.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>
#include <string>

namespace fs = std::filesystem;

namespace glia::app {

glia::core::CommandResult WasteScanCommand::execute(const std::vector<std::string>& args) {
    std::vector<std::pair<std::string, std::regex>> patterns = {
        {"TODO Placeholder", std::regex("TODO")},
        {"Skeletal Logic", std::regex("Implement .* logic", std::regex::icase)},
        {"Missing Implementation", std::regex("// \\.\\.\\.", std::regex::icase)},
        {"Boilerplate Warning", std::regex("boilerplate", std::regex::icase)}
    };

    std::vector<std::string> headers = {"File", "Line", "Pattern", "Snippet"};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext != ".cpp" && ext != ".h" && ext != ".hpp") continue;

            std::ifstream file(entry.path());
            std::string line;
            int lineNum = 0;
            while (std::getline(file, line)) {
                lineNum++;
                for (const auto& [name, regex] : patterns) {
                    if (std::regex_search(line, regex)) {
                        std::string snippet = line;
                        if (snippet.length() > 30) snippet = snippet.substr(0, 27) + "...";
                        rows.push_back({entry.path().filename().string(), std::to_string(lineNum), name, snippet});
                        break;
                    }
                }
            }
        }
    }

    if (rows.empty()) {
        return {glia::core::ExitCode::Success, "No LLM waste patterns detected"};
    }

    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Waste scan complete. " + std::to_string(rows.size()) + " patterns found."};
}

glia::core::CommandResult VerifyStructureCommand::execute(const std::vector<std::string>& args) {
    // Check for common SDD violations
    std::vector<std::pair<std::string, std::regex>> violations = {
        {"Potential Empty Catch", std::regex("catch\\s*\\([^\\)]*\\)\\s*\\{\\s*\\}")},
        {"Meaningless Assertion", std::regex("assert\\s*\\(\\s*(true|1)\\s*\\)")}
    };

    std::vector<std::string> headers = {"File", "Line", "Violation", "Context"};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext != ".cpp" && ext != ".h") continue;

            std::ifstream file(entry.path());
            std::string line;
            int lineNum = 0;
            while (std::getline(file, line)) {
                lineNum++;
                for (const auto& [name, regex] : violations) {
                    if (std::regex_search(line, regex)) {
                        rows.push_back({entry.path().filename().string(), std::to_string(lineNum), name, line});
                        break;
                    }
                }
            }
        }
    }

    if (rows.empty()) {
        return {glia::core::ExitCode::Success, "No structural violations detected"};
    }

    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Structure verification complete. " + std::to_string(rows.size()) + " violations found."};
}

}
