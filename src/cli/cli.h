#ifndef GLIA_CLI_CLI_H
#define GLIA_CLI_CLI_H
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace glia::cli {

class Terminal {
public:
    static void color(const std::string& code);
    static void reset();
    static void haptic();
    static void printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows, bool sortRows = false, int maxRows = 100);
};

class ArgumentParser {
public:
    void addOption(const std::string& name, const std::string& alias, const std::string& description);
    bool parse(int argc, char** argv);
    std::string getOption(const std::string& name) const;
    bool hasFlag(const std::string& name) const;
    std::vector<std::string> getPositional() const;
    void printHelp() const;
private:
    std::map<std::string, std::string> options;
    std::map<std::string, std::string> aliases;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, bool> flags;
    std::vector<std::string> positional;
};

class Prompter {
public:
    static bool confirm(const std::string& message);
    static std::string ask(const std::string& message);
};

}
#endif
