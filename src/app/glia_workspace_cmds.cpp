#include "glia_workspace_cmds.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include "command_loader.h"
#include <iostream>
#include <sys/wait.h>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;
using glia::util::Translator;

namespace glia::app {

namespace {
struct ExecResult {
    std::string output;
    int exitCode;
};
ExecResult exec(const char* cmd, const std::string& cwd = "") {
    std::array<char, 128> buffer;
    std::string result;
    std::string fullCmd = cmd;
    if (!cwd.empty()) fullCmd = "cd " + cwd + " && " + fullCmd;
    FILE* pipe = popen(fullCmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) result += buffer.data();
    int status = pclose(pipe);
    int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    return {result, exitCode};
}
std::string trim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}
}

glia::core::CommandResult WorkspaceStatusCommand::execute(const std::vector<std::string>& args) {
    std::string wsName = m_meta.params.count("workspace_dir") ? m_meta.params.at("workspace_dir") : "workspace";
    fs::path workspacePath = fs::current_path() / wsName;
    if (!fs::exists(workspacePath) || !fs::is_directory(workspacePath)) return {glia::core::ExitCode::FilesystemError, Translator::t("msg_error")};
    std::vector<std::string> headers = {Translator::t("col_repo"), Translator::t("col_branch"), Translator::t("col_sync"), Translator::t("col_diff")};
    std::vector<std::vector<std::string>> rows;
    for (const auto& entry : fs::directory_iterator(workspacePath)) {
        if (entry.is_directory() && fs::exists(entry.path() / ".git")) {
            std::string repoPath = entry.path().string();
            std::string branch = trim(exec("git rev-parse --abbrev-ref HEAD", repoPath).output);
            std::string local = trim(exec("git rev-parse @", repoPath).output);
            auto remoteRes = exec("git rev-parse @{u}", repoPath);
            std::string status;
            if (remoteRes.exitCode != 0) status = Translator::t("msg_none");
            else {
                std::string remote = trim(remoteRes.output);
                std::string base = trim(exec("git merge-base @ @{u}", repoPath).output);
                if (local == remote) status = Translator::t("msg_ok");
                else if (local == base) status = Translator::t("msg_behind");
                else if (remote == base) status = Translator::t("msg_ahead");
                else status = Translator::t("msg_div");
            }
            std::string changes = trim(exec("git status --porcelain", repoPath).output).empty() ? Translator::t("msg_no") : Translator::t("msg_yes");
            rows.push_back({entry.path().filename().string(), branch, status, changes});
        }
    }
    if (rows.empty()) return {glia::core::ExitCode::Success, Translator::t("msg_none")};
    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}

glia::core::CommandResult WorkspaceSyncCommand::execute(const std::vector<std::string>& args) {
    std::string wsName = m_meta.params.count("workspace_dir") ? m_meta.params.at("workspace_dir") : "workspace";
    fs::path workspacePath = fs::current_path() / wsName;
    if (!fs::exists(workspacePath) || !fs::is_directory(workspacePath)) return {glia::core::ExitCode::FilesystemError, Translator::t("msg_error")};
    int ok = 0, err = 0;
    std::vector<std::string> failures;
    for (const auto& entry : fs::directory_iterator(workspacePath)) {
        if (entry.is_directory() && fs::exists(entry.path() / ".git")) {
            exec("git fetch --all", entry.path().string());
            if (exec("git pull --rebase", entry.path().string()).exitCode == 0) ok++;
            else { err++; failures.push_back(entry.path().filename().string()); }
        }
    }
    if (err == 0) return {glia::core::ExitCode::Success, Translator::t("msg_synced")};
    auto res = glia::core::CommandResult{glia::core::ExitCode::PartialSuccess, Translator::t("msg_partial"), "", {}};
    for (const auto& f : failures) res.hints.push_back(f);
    return res;
}

glia::core::CommandResult WorkspaceMeshCommand::execute(const std::vector<std::string>& args) {
    std::cout << Translator::t("mesh_start") << "\n";
    std::string filename = m_meta.params.count("graph_file") ? m_meta.params.at("graph_file") : "graph.csv";
    std::string header = m_meta.params.count("graph_header") ? m_meta.params.at("graph_header") : "source,target,weight";
    std::string entry = m_meta.params.count("graph_entry") ? m_meta.params.at("graph_entry") : "multiple_viewer,greenhouse_org,1.0";
    std::ofstream out(filename);
    out << header << "\n" << entry << "\n";
    out.close();
    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}

}
