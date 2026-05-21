#ifndef GLIA_APP_GLIA_BUILD_CMDS_H
#define GLIA_APP_GLIA_BUILD_CMDS_H

#include "command.h"

namespace glia::app {

class BuildLintCmakeCommand : public Command {
public:
    std::string name() const override { return "build-lint-cmake"; }
    std::string description() const override { return "Check CMakeLists.txt for errors"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class TestUnitCommand : public Command {
public:
    std::string name() const override { return "test-unit"; }
    std::string description() const override { return "Run unit tests"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
