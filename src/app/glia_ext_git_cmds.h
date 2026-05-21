#ifndef GLIA_APP_GLIA_EXT_GIT_CMDS_H
#define GLIA_APP_GLIA_EXT_GIT_CMDS_H

#include "command.h"

namespace glia::app {

class GitCleanupLocalCommand : public Command {
public:
    std::string name() const override { return "git-cleanup-local"; }
    std::string description() const override { return "Delete local branches merged into main"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class GitFindCommitCommand : public Command {
public:
    std::string name() const override { return "git-find-commit"; }
    std::string description() const override { return "Search commit messages for a pattern"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
