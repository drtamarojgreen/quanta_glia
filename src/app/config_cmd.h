#ifndef GLIA_APP_CONFIG_CMD_H
#define GLIA_APP_CONFIG_CMD_H
#include "command.h"
namespace glia::app {
class ConfigCommand : public Command {
public:
    std::string name() const override { return "config"; }
    std::string description() const override { return "Manage system configuration"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
