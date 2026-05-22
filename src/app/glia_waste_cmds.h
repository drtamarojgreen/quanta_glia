#ifndef GLIA_APP_GLIA_WASTE_CMDS_H
#define GLIA_APP_GLIA_WASTE_CMDS_H

#include "command.h"

namespace glia::app {

class WasteScanCommand : public Command {
public:
    std::string name() const override { return "waste-scan"; }
    std::string description() const override { return "Scan for LLM-generated waste (TODOs, skeletal logic)"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class VerifyStructureCommand : public Command {
public:
    std::string name() const override { return "verify-structure"; }
    std::string description() const override { return "Verify code conforms to SDD structural restrictions"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
