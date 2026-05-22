#include "glia_waste_cmds.h"
#include "../cli/cli.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

namespace glia::app {

namespace {
std::string trim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}
}

glia::core::CommandResult WasteScanCommand::execute(const std::vector<std::string>& args) {
    std::vector<std::pair<std::string, std::regex>> patterns = {
        {"TODO Placeholder", std::regex("TODO")},
        {"Skeletal Logic", std::regex("Implement .* logic|Your logic here|Add implementation", std::regex::icase)},
        {"Missing Implementation", std::regex("// \\.\\.\\.|\\/\\* \\.\\.\\. \\*\\/|return \\{\\}; // placeholder", std::regex::icase)},
        {"Boilerplate Warning", std::regex("boilerplate|skeletal|placeholder", std::regex::icase)},
        {"Trivial Return", std::regex("\\{\\s*return\\s*[^;]*;\\s*\\}", std::regex::icase)},
        {"Repeated LLM Phrase", std::regex("common failure pattern|certainly, I can help|as an AI language model", std::regex::icase)}
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
        {"Meaningless Assertion", std::regex("assert\\s*\\(\\s*(true|1|!false|!!true)\\s*\\)")},
        {"Raw Pointer Usage", std::regex("\\b(new\\s+[^\\(;]+|delete\\s+[^;]+)\\b")},
        {"Magic String Placeholder", std::regex("\"(test|dummy|example|placeholder|stub)\"")},
        {"Magic Number Placeholder", std::regex("=\\s*(42|123|999|0xDEADBEEF|0xCAFEBABE)\\b")}
    };

    std::vector<std::string> headers = {"File", "Line", "Violation", "Context"};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext != ".cpp" && ext != ".h") continue;

            std::ifstream file(entry.path());
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            // Multiline check for empty catch
            // Matches catch(...) { [whitespace or comments] }
            // Using a simpler approach: find catch blocks and check if they are effectively empty
            std::regex catchPattern("catch\\s*\\([^\\)]*\\)\\s*\\{([^\\}]*)\\}");
            auto catch_begin = std::sregex_iterator(content.begin(), content.end(), catchPattern);
            auto catch_end = std::sregex_iterator();
            for (std::sregex_iterator i = catch_begin; i != catch_end; ++i) {
                std::smatch match = *i;
                std::string body = match[1].str();

                // Remove comments and whitespace from body
                std::string stripped = std::regex_replace(body, std::regex("\\/\\/[^\\n]*|\\/\\*.*?\\*\\/"), "");
                stripped.erase(std::remove_if(stripped.begin(), stripped.end(), ::isspace), stripped.end());

                if (stripped.empty()) {
                    int lineNum = std::count(content.begin(), content.begin() + match.position(), '\n') + 1;
                    rows.push_back({entry.path().filename().string(), std::to_string(lineNum), "Empty Catch Block", match.str().substr(0, 40)});
                }
            }

            // Line-based checks
            file.clear();
            file.seekg(0);
            std::string line;
            int lineNum = 0;
            while (std::getline(file, line)) {
                lineNum++;
                for (const auto& [name, regex] : violations) {
                    if (std::regex_search(line, regex)) {
                        rows.push_back({entry.path().filename().string(), std::to_string(lineNum), name, trim(line)});
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

glia::core::CommandResult AuditRepetitionCommand::execute(const std::vector<std::string>& args) {
    std::map<std::string, std::vector<std::pair<std::string, int>>> blockMap;
    std::vector<std::string> headers = {"Block Hash", "Files", "Snippet"};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext != ".cpp" && ext != ".h") continue;

            std::ifstream file(entry.path());
            std::string line;
            std::vector<std::string> lines;
            while (std::getline(file, line)) {
                std::string t = trim(line);
                if (!t.empty()) lines.push_back(t);
            }

            // Simple window-based repetition check
            const int windowSize = 3;
            if (lines.size() >= windowSize) {
                for (size_t i = 0; i <= lines.size() - windowSize; ++i) {
                    std::string block;
                    for (int j = 0; j < windowSize; ++j) block += lines[i+j] + "\n";
                    blockMap[block].push_back({entry.path().filename().string(), (int)i + 1});
                }
            }
        }
    }

    for (auto const& [block, occurrences] : blockMap) {
        if (occurrences.size() > 1) {
            // Check if occurrences are in different files or significantly different locations
            std::set<std::string> files;
            for (const auto& o : occurrences) files.insert(o.first);

            if (files.size() > 1) {
                std::string fileList;
                for (const auto& f : files) fileList += f + " ";
                std::string snippet = block.substr(0, block.find('\n'));
                rows.push_back({std::to_string(std::hash<std::string>{}(block)), fileList, snippet});
            }
        }
    }

    if (rows.empty()) {
        return {glia::core::ExitCode::Success, "No significant code repetition detected"};
    }

    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Repetition audit complete. Found " + std::to_string(rows.size()) + " repeated blocks."};
}

}
