#ifndef GLIA_APP_SHELL_COMMAND_H
#define GLIA_APP_SHELL_COMMAND_H

#include "command.h"
#include "../util/shell_utils.h"

namespace glia::app {

/**
 * A generic command class for simple shell-based operations.
 * Eliminates the need for multiple class definitions for basic wrappers.
 */
class GenericShellCommand : public Command {
public:
    GenericShellCommand(std::string name, std::string desc, std::string shellCmd)
        : m_name(std::move(name)), m_desc(std::move(desc)), m_shellCmd(std::move(shellCmd)) {}

    std::string name() const override { return m_name; }
    std::string description() const override { return m_desc; }

    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        // Simple parameter substitution if needed, or just append args
        std::string finalCmd = m_shellCmd;
        for (size_t i = 1; i < args.size(); ++i) {
            finalCmd += " " + args[i];
        }
        
        int code = glia::util::Shell::run(finalCmd);
        return glia::util::Shell::makeResult(code, m_name + " completed successfully", m_name + " failed");
    }

private:
    std::string m_name;
    std::string m_desc;
    std::string m_shellCmd;
};

}

#endif
