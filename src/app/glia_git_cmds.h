#ifndef GLIA_APP_GLIA_GIT_CMDS_H
#define GLIA_APP_GLIA_GIT_CMDS_H

#include "command.h"

namespace glia::app {

class CheckoutLatestCommand : public Command {
public:
    std::string name() const override { return "checkout-latest"; }
    std::string description() const override { return "Fetch latest and checkout the most recent branch"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class CommitChangesCommand : public Command {
public:
    std::string name() const override { return "commit-changes"; }
    std::string description() const override { return "Interactively add changes and commit"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class UpdateRepoCommand : public Command {
public:
    std::string name() const override { return "update-repo"; }
    std::string description() const override { return "Fetch, pull --rebase, and push"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class QuickCommitCommand : public Command {
public:
    std::string name() const override { return "quick-commit"; }
    std::string description() const override { return "Stage all and commit with message"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
