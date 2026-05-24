#ifndef GLIA_APP_GLIA_SCORE_CMDS_H
#define GLIA_APP_GLIA_SCORE_CMDS_H
#include "command.h"
#include <map>
#include <string>
#include <vector>

namespace glia::app {

class RestrictionsCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class ScoreCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}
#endif
