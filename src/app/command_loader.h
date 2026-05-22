#ifndef GLIA_APP_COMMAND_LOADER_H
#define GLIA_APP_COMMAND_LOADER_H

#include <string>
#include <vector>

namespace glia::app {

struct CommandMetadata {
    std::string name;
    std::string description;
    std::string type;
    std::string target;
};

class CommandLoader {
public:
    static std::vector<CommandMetadata> loadFromXml(const std::string& path);
};

}

#endif
