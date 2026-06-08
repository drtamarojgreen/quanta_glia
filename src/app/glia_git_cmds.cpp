#include "glia_git_cmds.h"
#include "../cli/cli.h"
#include "../util/shell_utils.h"
#include <iostream>
#include <sstream>

using glia::util::exec;

namespace glia::app {

namespace {
int run(const std::string& cmd) {
    return std::system(cmd.c_str());
}
}

glia::core::CommandResult CheckoutLatestCommand::execute(const std::vector<std::string>& args) {
    if (run("git fetch --all") != 0) return {glia::core::ExitCode::InternalFailure, ""};
    std::string branches = exec("git branch -a --sort=-committerdate --format='%(refname:short)'").output;
    std::stringstream ss(branches);
    std::string latest;
    if (std::getline(ss, latest)) {
        if (latest.find("origin/") == 0) latest = latest.substr(7);
        if (run("git checkout " + latest) == 0) return {glia::core::ExitCode::Success, ""};
    }
    return {glia::core::ExitCode::InternalFailure, ""};
}

glia::core::CommandResult CommitChangesCommand::execute(const std::vector<std::string>& args) {
    std::string changed = exec("git diff --name-only").output;
    std::stringstream ss(changed);
    std::string file;
    while (std::getline(ss, file)) {
        if (!file.empty() && glia::cli::Prompter::confirm(file)) run("git add " + file);
    }
    std::string msg = args.size() > 1 ? args[1] : "";
    if (!msg.empty() && run("git commit -m \"" + msg + "\"") == 0) return {glia::core::ExitCode::Success, ""};
    return {glia::core::ExitCode::Success, ""};
}

glia::core::CommandResult UpdateRepoCommand::execute(const std::vector<std::string>& args) {
    if (run("git fetch") == 0 && run("git pull --rebase") == 0 && run("git push") == 0)
        return {glia::core::ExitCode::Success, ""};
    return {glia::core::ExitCode::InternalFailure, ""};
}

glia::core::CommandResult QuickCommitCommand::execute(const std::vector<std::string>& args) {
    run("git add .");
    std::string msg = args.size() > 1 ? args[1] : "";
    if (msg.empty() && m_meta.params.count("default_message")) msg = m_meta.params.at("default_message");
    if (msg.empty()) msg = glia::cli::Prompter::ask("Msg");
    std::string escaped;
    for (char c : msg) {
        if (c == '"' || c == '\\' || c == '$' || c == '`') escaped += '\\';
        escaped += c;
    }
    if (run("git commit -m \"" + escaped + "\"") == 0) return {glia::core::ExitCode::Success, ""};
    return {glia::core::ExitCode::InternalFailure, ""};
}

glia::core::CommandResult GliaAddCommand::execute(const std::vector<std::string>& args) {
    std::string changed = exec("git diff --name-only").output;
    std::stringstream ss(changed);
    std::string file;
    while (std::getline(ss, file)) {
        if (!file.empty() && glia::cli::Prompter::confirm("Stage " + file + "?")) {
            run("git add " + file);
        }
    }
    return {glia::core::ExitCode::Success, "Modified files added"};
}

glia::core::CommandResult GliaAddAllCommand::execute(const std::vector<std::string>& args) {
    std::string status = exec("git status --porcelain").output;
    std::stringstream ss(status);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.length() < 4) continue;
        std::string file = line.substr(3);
        if (glia::cli::Prompter::confirm("Stage " + file + "?")) {
            run("git add " + file);
        }
    }
    return {glia::core::ExitCode::Success, "Modified and untracked files added"};
}

}
