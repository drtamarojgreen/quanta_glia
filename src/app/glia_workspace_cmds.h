#ifndef GLIA_APP_GLIA_WORKSPACE_CMDS_H
#define GLIA_APP_GLIA_WORKSPACE_CMDS_H
#include "command.h"
namespace glia::app {
class WorkspaceStatusCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
class WorkspaceSyncCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
class WorkspaceMeshCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
