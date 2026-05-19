#ifndef GLIA_APP_HARVEST_CMD_H
#define GLIA_APP_HARVEST_CMD_H
#include "command.h"
namespace glia::app {
class HarvestCommand : public Command {
public:
    std::string name() const override { return "harvest"; }
    std::string description() const override { return "Clone and extract information from repos"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
