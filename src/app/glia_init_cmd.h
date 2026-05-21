#ifndef GLIA_APP_GLIA_INIT_CMD_H
#define GLIA_APP_GLIA_INIT_CMD_H

#include "command.h"

namespace glia::app {

class GliaInitCommand : public Command {
public:
    std::string name() const override { return "init"; }
    std::string description() const override { return "Add glia to the path"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
