#include "glia_workspace_cmds.h"
#include "../cli/cli.h"
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

namespace fs = std::filesystem;

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
    if (!cwd.empty()) {
        fullCmd = "cd " + cwd + " && " + fullCmd;
    }
    FILE* pipe = popen(fullCmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    int status = pclose(pipe);
    int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    return {result, exitCode};
}

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

glia::core::CommandResult WorkspaceStatusCommand::execute(const std::vector<std::string>& args) {
    fs::path workspacePath = fs::current_path() / "workspace";
    if (!fs::exists(workspacePath) || !fs::is_directory(workspacePath)) {
        return {glia::core::ExitCode::FilesystemError, "workspace/ directory not found"};
    }

    std::vector<std::string> headers = {"Repository", "Branch", "Status", "Changes"};
    std::vector<std::vector<std::string>> rows;

    for (const auto& entry : fs::directory_iterator(workspacePath)) {
        if (entry.is_directory() && fs::exists(entry.path() / ".git")) {
            std::string repoName = entry.path().filename().string();
            std::string repoPath = entry.path().string();

            std::string branch = trim(exec("git rev-parse --abbrev-ref HEAD", repoPath).output);

            // Check status against upstream (No fetch to avoid network delay/hangs)
            std::string local = trim(exec("git rev-parse @", repoPath).output);
            auto remoteRes = exec("git rev-parse @{u}", repoPath);
            std::string status;

            if (remoteRes.exitCode != 0) {
                status = "No Upstream";
            } else {
                std::string remote = trim(remoteRes.output);
                std::string base = trim(exec("git merge-base @ @{u}", repoPath).output);

                if (local == remote) status = "Up to date";
                else if (local == base) status = "Behind";
                else if (remote == base) status = "Ahead";
                else status = "Diverged";
            }

            std::string changes = trim(exec("git status --porcelain", repoPath).output);
            std::string changesStr = changes.empty() ? "Clean" : "Modified";

            rows.push_back({repoName, branch, status, changesStr});
        }
    }

    if (rows.empty()) {
        return {glia::core::ExitCode::Success, "No git repositories found in workspace/"};
    }

    glia::cli::Terminal::printTable(headers, rows);
    return {glia::core::ExitCode::Success, "Workspace status displayed"};
}

glia::core::CommandResult WorkspaceSyncCommand::execute(const std::vector<std::string>& args) {
    fs::path workspacePath = fs::current_path() / "workspace";
    if (!fs::exists(workspacePath) || !fs::is_directory(workspacePath)) {
        return {glia::core::ExitCode::FilesystemError, "workspace/ directory not found"};
    }

    int successCount = 0;
    int failCount = 0;
    std::vector<std::string> failures;

    for (const auto& entry : fs::directory_iterator(workspacePath)) {
        if (entry.is_directory() && fs::exists(entry.path() / ".git")) {
            std::string repoName = entry.path().filename().string();
            std::string repoPath = entry.path().string();

            std::cout << "Syncing " << repoName << "..." << std::endl;

            auto fetchRes = exec("git fetch --all", repoPath);
            auto pullRes = exec("git pull --rebase", repoPath);

            if (pullRes.exitCode == 0) {
                successCount++;
            } else {
                failCount++;
                failures.push_back(repoName);
            }
        }
    }

    glia::core::CommandResult result;
    if (failCount == 0) {
        result.code = glia::core::ExitCode::Success;
        result.userMessage = "All " + std::to_string(successCount) + " repositories synced successfully";
    } else {
        result.code = glia::core::ExitCode::PartialSuccess;
        result.userMessage = "Synced " + std::to_string(successCount) + " repositories, " + std::to_string(failCount) + " failed";
        for (const auto& f : failures) result.hints.push_back("Failed to sync: " + f);
    }
    return result;
}

}
