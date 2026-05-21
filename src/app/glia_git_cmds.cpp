#include "glia_git_cmds.h"
#include "../cli/cli.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>

namespace glia::app {

namespace {
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int run(const std::string& cmd) {
    std::cout << "Executing: " << cmd << std::endl;
    return std::system(cmd.c_str());
}
}

glia::core::CommandResult CheckoutLatestCommand::execute(const std::vector<std::string>& args) {
    if (run("git fetch --all") != 0) {
        return {glia::core::ExitCode::InternalFailure, "git fetch failed"};
    }
    
    std::string branches = exec("git branch -a --sort=-committerdate --format='%(refname:short)'");
    std::stringstream ss(branches);
    std::string latestBranch;
    if (std::getline(ss, latestBranch)) {
        // Strip origin/ if it's a remote branch
        if (latestBranch.find("origin/") == 0) {
            latestBranch = latestBranch.substr(7);
        }
        std::string checkoutCmd = "git checkout " + latestBranch;
        if (run(checkoutCmd) == 0) {
            return {glia::core::ExitCode::Success, "Checked out latest branch: " + latestBranch};
        } else {
            return {glia::core::ExitCode::InternalFailure, "git checkout failed"};
        }
    }
    
    return {glia::core::ExitCode::InternalFailure, "No branches found"};
}

glia::core::CommandResult CommitChangesCommand::execute(const std::vector<std::string>& args) {
    std::string changedFiles = exec("git diff --name-only");
    std::stringstream ss(changedFiles);
    std::string file;
    int addedCount = 0;
    
    while (std::getline(ss, file)) {
        if (file.empty()) continue;
        if (glia::cli::Prompter::confirm("Add " + file + "?")) {
            if (run("git add " + file) == 0) {
                addedCount++;
            }
        }
    }
    
    std::string message;
    if (args.size() > 1) {
        message = args[1];
    }
    
    if (!message.empty()) {
        if (run("git commit -m \"" + message + "\"") == 0) {
            return {glia::core::ExitCode::Success, "Committed " + std::to_string(addedCount) + " files with message: " + message};
        } else {
            return {glia::core::ExitCode::InternalFailure, "git commit failed"};
        }
    }
    
    return {glia::core::ExitCode::Success, "Added " + std::to_string(addedCount) + " files. No commit message provided."};
}

glia::core::CommandResult UpdateRepoCommand::execute(const std::vector<std::string>& args) {
    if (run("git fetch") != 0) {
        return {glia::core::ExitCode::InternalFailure, "git fetch failed"};
    }
    
    if (run("git pull --rebase") != 0) {
        return {glia::core::ExitCode::InternalFailure, "git pull --rebase failed"};
    }
    
    if (run("git push") == 0) {
        return {glia::core::ExitCode::Success, "Repo updated and pushed successfully"};
    } else {
        return {glia::core::ExitCode::InternalFailure, "git push failed"};
    }
}

glia::core::CommandResult QuickCommitCommand::execute(const std::vector<std::string>& args) {
    if (run("git add .") != 0) {
        return {glia::core::ExitCode::InternalFailure, "git add . failed"};
    }

    std::string message = "Quick commit";
    if (args.size() > 1) {
        message = args[1];
    } else {
        message = glia::cli::Prompter::ask("Commit message");
    }

    if (message.empty()) message = "Quick commit";

    // Escape double quotes for shell safety
    std::string escapedMessage;
    for (char c : message) {
        if (c == '"' || c == '\\' || c == '$' || c == '`') escapedMessage += '\\';
        escapedMessage += c;
    }

    std::string commitCmd = "git commit -m \"" + escapedMessage + "\"";
    if (run(commitCmd) == 0) {
        return {glia::core::ExitCode::Success, "Successfully committed with message: " + message};
    } else {
        return {glia::core::ExitCode::InternalFailure, "git commit failed"};
    }
}

}
