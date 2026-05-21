#ifndef GLIA_APP_GLIA_ENV_CMDS_H
#define GLIA_APP_GLIA_ENV_CMDS_H

#include "command.h"

namespace glia::app {

class EnvShellCommand : public Command {
public:
    std::string name() const override { return "env-shell"; }
    std::string description() const override { return "Open shell in a container service"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class EnvSetVarCommand : public Command {
public:
    std::string name() const override { return "env-set-var"; }
    std::string description() const override { return "Set environment variable in local config"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class EnvListVarsCommand : public Command {
public:
    std::string name() const override { return "env-list-vars"; }
    std::string description() const override { return "List all environment variables"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class EnvCleanVolumesCommand : public Command {
public:
    std::string name() const override { return "env-clean-volumes"; }
    std::string description() const override { return "Remove unused Docker volumes"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class EnvDoctorCommand : public Command {
public:
    std::string name() const override { return "env-doctor"; }
    std::string description() const override { return "Diagnose environment issues"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
