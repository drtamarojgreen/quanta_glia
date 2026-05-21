#ifndef GLIA_APP_GLIA_WORKSPACE_CMDS_H
#define GLIA_APP_GLIA_WORKSPACE_CMDS_H

#include "command.h"

namespace glia::app {

class WorkspaceStatusCommand : public Command {
public:
    std::string name() const override { return "workspace-status"; }
    std::string description() const override { return "Show status of all repositories in workspace/"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class WorkspaceSyncCommand : public Command {
public:
    std::string name() const override { return "workspace-sync"; }
    std::string description() const override { return "Sync (fetch & rebase) all repositories in workspace/"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
