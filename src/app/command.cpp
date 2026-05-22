#include "command.h"
#include <algorithm>
#include <iostream>

namespace glia::app {
void CommandRegistry::registerCommand(std::unique_ptr<Command> cmd) {
    commands.push_back(std::move(cmd));
}
Command* CommandRegistry::getCommand(const std::string& name) {
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

glia::core::CommandResult ExternalCommand::execute(const std::vector<std::string>& args) {
    std::cout << "Executing external command: " << m_target << std::endl;
    int res = std::system(m_target.c_str());
    if (res == 0) return {glia::core::ExitCode::Success, "External command executed successfully"};
    return {glia::core::ExitCode::InternalFailure, "External command failed with exit code " + std::to_string(res)};
}
}
