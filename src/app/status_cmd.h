#ifndef GLIA_APP_STATUS_CMD_H
#define GLIA_APP_STATUS_CMD_H
#include "command.h"
namespace glia::app {
class StatusCommand : public Command {
public:
    std::string name() const override { return "status"; }
    std::string description() const override { return "Show system status dashboard"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
