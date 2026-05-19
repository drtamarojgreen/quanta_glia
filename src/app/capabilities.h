#ifndef GLIA_APP_CAPABILITIES_H
#define GLIA_APP_CAPABILITIES_H
#include "command.h"
namespace glia::app {
class CapabilitiesCommand : public Command {
public:
    std::string name() const override { return "capabilities"; }
    std::string description() const override { return "List available Glia functions"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
