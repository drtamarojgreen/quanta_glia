#ifndef GLIA_APP_ABOUT_H
#define GLIA_APP_ABOUT_H
#include "command.h"
namespace glia::app {
class AboutCommand : public Command {
public:
    std::string name() const override { return "about"; }
    std::string description() const override { return "Display application information"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
