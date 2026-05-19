#ifndef GLIA_CLI_CLI_H
#define GLIA_CLI_CLI_H
#include <string>
#include <vector>
#include <map>

namespace glia::cli {
class ArgumentParser {
public:
    void addOption(const std::string& name, const std::string& alias);
    bool parse(int argc, char** argv);
    std::string getOption(const std::string& name) const;
    bool hasFlag(const std::string& name) const;
    std::vector<std::string> getPositional() const;
private:
    std::map<std::string, std::string> options;
    std::map<std::string, std::string> aliases;
    std::map<std::string, bool> flags;
    std::vector<std::string> positional;
};
}
#endif
