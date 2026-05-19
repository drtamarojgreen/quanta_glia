#include "cli.h"
#include <algorithm>

namespace glia::cli {
void ArgumentParser::addOption(const std::string& name, const std::string& alias) {
    aliases[alias] = name;
}
bool ArgumentParser::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.size() > 1 && arg[0] == '-') {
            std::string name = arg;
            if (aliases.count(name)) name = aliases[name];
            if (i + 1 < argc && argv[i+1][0] != '-') {
                options[name] = argv[++i];
            } else {
                flags[name] = true;
            }
        } else {
            positional.push_back(arg);
        }
    }
    return true;
}
std::string ArgumentParser::getOption(const std::string& name) const { return options.count(name) ? options.at(name) : ""; }
bool ArgumentParser::hasFlag(const std::string& name) const { return flags.count(name); }
std::vector<std::string> ArgumentParser::getPositional() const { return positional; }
}
