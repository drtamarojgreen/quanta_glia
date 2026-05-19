#ifndef GLIA_APP_PRUNE_CMD_H
#define GLIA_APP_PRUNE_CMD_H
#include "command.h"
namespace glia::app {
class PruneCommand : public Command {
public:
    std::string name() const override { return "prune"; }
    std::string description() const override { return "Evaluate and prune the knowledge base"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
