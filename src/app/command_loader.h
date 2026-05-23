#ifndef GLIA_APP_COMMAND_LOADER_H
#define GLIA_APP_COMMAND_LOADER_H

#include <string>
#include <vector>
#include <map>

namespace glia::app {

struct CommandMetadata {
    std::string name;
    std::string description;
    std::string type;
    std::string target;
    std::map<std::string, std::string> params;
    std::map<std::string, std::vector<std::string>> lists;
};

struct RuleGlobals {
    std::vector<std::string> extensions;
    std::vector<std::string> ignoredDirs;
    std::map<std::string, std::string> uiStrings;
};

class CommandLoader {
public:
    static std::vector<CommandMetadata> loadFromXml(const std::string& path);
    static RuleGlobals loadGlobals(const std::string& path);
};

}

#endif
