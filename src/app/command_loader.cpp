#include "command_loader.h"
#include <fstream>
#include <iostream>
#include <regex>

namespace glia::app {

std::vector<CommandMetadata> CommandLoader::loadFromXml(const std::string& path) {
    std::vector<CommandMetadata> commands;
    std::ifstream file(path);
    if (!file.is_open()) {
        // Fallback to rules/rules.xml if absolute path not found
        file.open("../" + path);
        if (!file.is_open()) {
            std::cerr << "Could not open rules file: " << path << std::endl;
            return commands;
        }
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Improved regex to handle XML tags properly, capturing all attributes
    std::regex cmdRegex("<command\\s+name=\"([^\"]+)\"\\s+description=\"([^\"]+)\"\\s+type=\"([^\"]+)\"\\s+target=\"([^\"]+)\"\\s*/>");
    auto cmd_begin = std::sregex_iterator(content.begin(), content.end(), cmdRegex);
    auto cmd_end = std::sregex_iterator();

    for (std::sregex_iterator i = cmd_begin; i != cmd_end; ++i) {
        std::smatch match = *i;
        commands.push_back({match[1].str(), match[2].str(), match[3].str(), match[4].str()});
    }

    return commands;
}

}
