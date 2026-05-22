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
        return std::make_unique<ExternalCommand>(meta.name, meta.description, meta.target);
    }
    if (creators().count(meta.target)) {
        return creators()[meta.target]();
    }
    return nullptr;
}

}
