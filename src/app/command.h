#ifndef GLIA_APP_COMMAND_H
#define GLIA_APP_COMMAND_H
#include "../core/result.h"
#include "command_loader.h"
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
    virtual void configure(const CommandMetadata& meta) { m_meta = meta; }
protected:
    CommandMetadata m_meta;
};

class CommandRegistry {
public:
    void registerCommand(std::unique_ptr<Command> cmd);
    Command* getCommand(const std::string& name) const;
    std::vector<std::string> listCommands() const;
private:
    std::vector<std::unique_ptr<Command>> commands;
};

class ExternalCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
