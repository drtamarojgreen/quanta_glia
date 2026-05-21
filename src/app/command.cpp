#include "command.h"
#include <algorithm>

namespace glia::app {
void CommandRegistry::registerCommand(std::unique_ptr<Command> cmd) {
    commands.push_back(std::move(cmd));
}
Command* CommandRegistry::getCommand(const std::string& name) const {
    for (auto& cmd : commands) {
        if (cmd->name() == name) return cmd.get();
    }
    return nullptr;
}
std::vector<std::string> CommandRegistry::listCommands() const {
    std::vector<std::string> names;
    for (auto& cmd : commands) names.push_back(cmd->name());
    return names;
}
}
