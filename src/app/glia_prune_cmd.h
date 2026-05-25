#ifndef GLIA_APP_GLIA_PRUNE_CMD_H
#define GLIA_APP_GLIA_PRUNE_CMD_H

#include "command.h"

namespace glia::app {

class PruneCurrentCommand : public Command {
public:
    std::string name() const override { return "prune-current"; }
    std::string description() const override { return "Interactively manage untracked files and directories"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
