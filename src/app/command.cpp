#include "command.h"
#include "../util/translator.h"
#include <algorithm>
#include <iostream>

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

glia::core::CommandResult ExternalCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    std::string target = m_meta.target;
    if (target.empty()) return {glia::core::ExitCode::ConfigError, Translator::t("msg_error")};

    int res = std::system(target.c_str());
    if (res == 0) return {glia::core::ExitCode::Success, Translator::t("msg_done")};
    return {glia::core::ExitCode::InternalFailure, std::to_string(res)};
}
}
