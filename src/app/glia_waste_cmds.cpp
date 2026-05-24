#include "glia_waste_cmds.h"
#include "../cli/cli.h"
#include "../util/string_utils.h"
#include "../util/translator.h"
#include "command_loader.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <set>
#include <array>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;
using glia::util::Translator;

namespace glia::app {

glia::core::CommandResult WasteScanCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");

    std::vector<std::pair<std::string, std::regex>> patterns;
    if (m_meta.lists.count("patterns")) {
        for (const auto& p : m_meta.lists.at("patterns")) {
            patterns.push_back({"Match", std::regex(p, std::regex::icase)});
        }
    }

    std::vector<std::string> headers = {Translator::t("col_file"), Translator::t("col_line"), Translator::t("col_snippet")};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        bool skip = false;
        for (const auto& ignored : globals.ignoredDirs) {
             if (!ignored.empty() && entry.path().string().find(ignored) != std::string::npos) { skip = true; break; }
        }
        if (skip || !entry.is_regular_file()) continue;

        std::string ext = entry.path().extension().string();
        bool validExt = false;
        for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
        if (!validExt) continue;

        std::ifstream file(entry.path());
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            for (const auto& [name, regex] : patterns) {
                if (std::regex_search(line, regex)) {
                    std::string snippet = line;
                    if (snippet.length() > 30) snippet = snippet.substr(0, 27) + "...";
                    rows.push_back({entry.path().filename().string(), std::to_string(lineNum), snippet});
                    break;
                }
            }
        }
    }

    if (rows.empty()) return {glia::core::ExitCode::Success, Translator::t("msg_clear")};
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_audit_complete")};
}

glia::core::CommandResult VerifyStructureCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    std::vector<std::pair<std::string, std::regex>> violations;
    if (m_meta.lists.count("violations")) {
        for (const auto& v : m_meta.lists.at("violations")) {
            size_t sep = v.find('|');
            if (sep != std::string::npos) {
                violations.push_back({v.substr(0, sep), std::regex(v.substr(sep+1), std::regex::extended)});
            }
        }
    }

    std::vector<std::string> headers = {Translator::t("col_file"), Translator::t("col_line"), Translator::t("col_type"), Translator::t("col_snippet")};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        bool skip = false;
        for (const auto& ignored : globals.ignoredDirs) if (!ignored.empty() && entry.path().string().find(ignored) != std::string::npos) { skip = true; break; }
        if (skip || !entry.is_regular_file()) continue;

        std::string ext = entry.path().extension().string();
        bool validExt = false;
        for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
        if (!validExt) continue;

        std::ifstream file(entry.path());
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        for (const auto& [name, regex] : violations) {
            if (name.find("Catch") != std::string::npos) {
                auto catch_begin = std::sregex_iterator(content.begin(), content.end(), regex);
                auto catch_end = std::sregex_iterator();
                for (std::sregex_iterator i = catch_begin; i != catch_end; ++i) {
                    std::smatch match = *i;
                    std::string body = match[1].str();
                    std::string stripped = std::regex_replace(body, std::regex("\\/\\/[^\\n]*|\\/\\*.*?\\*\\/"), "");
                    stripped.erase(std::remove_if(stripped.begin(), stripped.end(), ::isspace), stripped.end());
                    if (stripped.empty()) {
                        int lineNum = std::count(content.begin(), content.begin() + match.position(), '\n') + 1;
                        rows.push_back({entry.path().filename().string(), std::to_string(lineNum), name, match.str().substr(0, 40)});
                    }
                }
            }
        }

        file.clear();
        file.seekg(0);
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            for (const auto& [name, regex] : violations) {
                if (name.find("Catch") != std::string::npos) continue;
                if (std::regex_search(line, regex)) {
                    rows.push_back({entry.path().filename().string(), std::to_string(lineNum), name, glia::util::trim(line)});
                    break;
                }
            }
        }
    }

    if (rows.empty()) return {glia::core::ExitCode::Success, Translator::t("msg_clear")};
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_audit_complete")};
}

glia::core::CommandResult AuditRepetitionCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    std::map<std::string, std::vector<std::pair<std::string, int>>> blockMap;
    std::vector<std::string> headers = {Translator::t("col_hash"), Translator::t("col_files"), Translator::t("col_snippet")};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        bool skip = false;
        for (const auto& ignored : globals.ignoredDirs) if (!ignored.empty() && entry.path().string().find(ignored) != std::string::npos) { skip = true; break; }
        if (skip || !entry.is_regular_file()) continue;

        std::string ext = entry.path().extension().string();
        bool validExt = false;
        for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
        if (!validExt) continue;

        std::ifstream file(entry.path());
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(file, line)) {
            std::string t = glia::util::trim(line);
            if (!t.empty()) lines.push_back(t);
        }

        const int windowSize = 3;
        if (lines.size() >= windowSize) {
            for (size_t i = 0; i <= lines.size() - windowSize; ++i) {
                std::string block;
                for (int j = 0; j < windowSize; ++j) block += lines[i+j] + "\n";
                blockMap[block].push_back({entry.path().filename().string(), (int)i + 1});
            }
        }
    }

    for (auto const& [block, occurrences] : blockMap) {
        if (occurrences.size() > 1) {
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

    if (rows.empty()) return {glia::core::ExitCode::Success, Translator::t("msg_clear")};
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_audit_complete")};
}

glia::core::CommandResult AuditCommitsCommand::execute(const std::vector<std::string>& args) {
    auto globals = CommandLoader::loadGlobals("rules/rules.xml");
    std::vector<std::string> keywords;
    if (m_meta.lists.count("keywords")) keywords = m_meta.lists.at("keywords");

    int limit = 50;
    if (m_meta.params.count("log_limit")) limit = std::stoi(m_meta.params.at("log_limit"));

    std::vector<std::string> headers = {Translator::t("col_hash"), Translator::t("col_date"), Translator::t("col_message")};
    std::vector<std::vector<std::string>> rows;

    std::string logCmd = "git log -n " + std::to_string(limit) + " --pretty=format:\"%h|%ad|%s\" --date=short";

    auto execLog = [](const char* cmd) -> std::string {
        std::array<char, 128> buffer;
        std::string result;
        FILE* pipe = popen(cmd, "r");
        if (pipe) {
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) result += buffer.data();
            pclose(pipe);
        }
        return result;
    };

    std::string logOutput = execLog(logCmd.c_str());
    std::stringstream ss(logOutput);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        std::string lineLower = line;
        std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(), ::tolower);
        bool suspicious = false;
        for (const auto& kw : keywords) { if (lineLower.find(kw) != std::string::npos) { suspicious = true; break; } }

        if (suspicious) {
            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos) {
                std::string msg = line.substr(p2 + 1);
                if (msg.length() > 40) msg = msg.substr(0, 37) + "...";
                rows.push_back({line.substr(0, p1), line.substr(p1 + 1, p2 - p1 - 1), msg});
            }
        }
    }

    if (rows.empty()) return {glia::core::ExitCode::Success, Translator::t("msg_clear")};
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_audit_complete")};
}

}
