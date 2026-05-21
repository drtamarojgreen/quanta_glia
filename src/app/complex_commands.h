#ifndef GLIA_APP_COMPLEX_COMMANDS_H
#define GLIA_APP_COMPLEX_COMMANDS_H

#include "command.h"
#include "../util/shell_utils.h"
#include "../cli/cli.h"
#include <sstream>
#include <filesystem>
#include <fstream>

namespace glia::app {

/**
 * Commands that require interactive logic, multi-step filesystem operations,
 * or complex string parsing (not suitable for GenericShellCommand).
 */

class GitCleanupLocalCommand : public Command {
public:
    std::string name() const override { return "git-cleanup-local"; }
    std::string description() const override { return "Delete local branches merged into main"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        std::string branches = glia::util::Shell::query("git branch --merged main | grep -v '^*' | grep -v 'main'");
        std::stringstream ss(branches);
        std::string branch;
        int count = 0;
        while (std::getline(ss, branch)) {
            branch = glia::util::Shell::query("echo " + branch + " | xargs"); // trim
            if (branch.empty()) continue;
            if (glia::cli::Prompter::confirm("Delete merged branch '" + branch + "'?")) {
                if (glia::util::Shell::run("git branch -d " + branch) == 0) count++;
            }
        }
        return {glia::core::ExitCode::Success, "Cleaned up " + std::to_string(count) + " branches"};
    }
};

class DocTodoListCommand : public Command {
public:
    std::string name() const override { return "doc-todo-list"; }
    std::string description() const override { return "Extract TODOs from codebase into report"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        std::string todos = glia::util::Shell::query("grep -rnE 'TODO|FIXME' src include");
        std::ofstream report("TODO_REPORT.md");
        report << "# Project TODOs\n\n" << todos << "\n";
        return {glia::core::ExitCode::Success, "TODO report generated in TODO_REPORT.md"};
    }
};

class SysAliasSetupCommand : public Command {
public:
    std::string name() const override { return "sys-alias-setup"; }
    std::string description() const override { return "Setup shell aliases for glia"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        char* home = std::getenv("HOME");
        if (!home) return {glia::core::ExitCode::InternalFailure, "HOME not found"};
        std::string path = std::string(home) + "/.bash_aliases";
        std::ofstream aliases(path, std::ios::app);
        aliases << "\nalias g='glia'\nalias gc='glia commit-changes'\nalias gu='glia update-repo'\n";
        return {glia::core::ExitCode::Success, "Aliases added to ~/.bash_aliases"};
    }
};

class ProjEstimateCommand : public Command {
public:
    std::string name() const override { return "proj-estimate"; }
    std::string description() const override { return "Estimate project size (LOC, files)"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        std::string stats = glia::util::Shell::query("find src include -type f | xargs wc -l | tail -n 1");
        return {glia::core::ExitCode::Success, "Project Estimate: " + stats};
    }
};

}

#endif
