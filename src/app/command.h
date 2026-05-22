#ifndef GLIA_APP_COMMAND_H
#define GLIA_APP_COMMAND_H
#include "../core/result.h"
#include <string>
#include <vector>
#include <memory>

namespace glia::app {
class Command {
public:
    virtual ~Command() = default;
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual glia::core::CommandResult execute(const std::vector<std::string>& args) = 0;
};

class CommandRegistry {
public:
    void registerCommand(std::unique_ptr<Command> cmd);
    Command* getCommand(const std::string& name);
    std::vector<std::string> listCommands() const;
private:
    std::vector<std::unique_ptr<Command>> commands;
};

class ExternalCommand : public Command {
public:
    ExternalCommand(std::string name, std::string desc, std::string target)
        : m_name(name), m_desc(desc), m_target(target) {}
    std::string name() const override { return m_name; }
    std::string description() const override { return m_desc; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
private:
    std::string m_name;
    std::string m_desc;
    std::string m_target;
};

}
#endif
