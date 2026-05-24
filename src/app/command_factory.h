#ifndef GLIA_APP_COMMAND_FACTORY_H
#define GLIA_APP_COMMAND_FACTORY_H

#include "command.h"
#include "command_loader.h"
#include <memory>
#include <map>
#include <functional>

namespace glia::app {

class CommandFactory {
public:
    using Creator = std::function<std::unique_ptr<Command>()>;

    static void registerCreator(const std::string& targetName, Creator creator);
    static std::unique_ptr<Command> create(const CommandMetadata& meta);

private:
    static std::map<std::string, Creator>& creators();
};

}

#endif
