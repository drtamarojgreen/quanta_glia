#include "command_factory.h"

namespace glia::app {

std::map<std::string, CommandFactory::Creator>& CommandFactory::creators() {
    static std::map<std::string, Creator> instance;
    return instance;
}

void CommandFactory::registerCreator(const std::string& targetName, Creator creator) {
    creators()[targetName] = creator;
}

std::unique_ptr<Command> CommandFactory::create(const CommandMetadata& meta) {
    if (meta.type == "external") {
        auto cmd = std::make_unique<ExternalCommand>();
        cmd->configure(meta);
        return cmd;
    }
    if (creators().count(meta.target)) {
        auto cmd = creators()[meta.target]();
        cmd->configure(meta);
        return cmd;
    }
    return nullptr;
}

}
