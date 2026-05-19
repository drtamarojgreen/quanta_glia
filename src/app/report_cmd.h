#ifndef GLIA_APP_REPORT_CMD_H
#define GLIA_APP_REPORT_CMD_H
#include "command.h"
namespace glia::app {
class ReportCommand : public Command {
public:
    std::string name() const override { return "report"; }
    std::string description() const override { return "Generate analytics report"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
