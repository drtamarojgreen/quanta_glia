#ifndef GLIA_APP_SEQUENCE_COMMAND_H
#define GLIA_APP_SEQUENCE_COMMAND_H

#include "command.h"
#include "../util/shell_utils.h"
#include <vector>

namespace glia::app {

/**
 * Executes a sequence of dependent shell commands.
 * If any step fails (non-zero exit code), execution stops.
 */
class SequenceCommand : public Command {
public:
    SequenceCommand(std::string name, std::string desc, std::vector<std::string> steps)
        : m_name(std::move(name)), m_desc(std::move(desc)), m_steps(std::move(steps)) {}

    std::string name() const override { return m_name; }
    std::string description() const override { return m_desc; }

    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        std::cout << "Starting sequence: " << m_name << std::endl;
        
        for (size_t i = 0; i < m_steps.size(); ++i) {
            std::cout << "[" << (i + 1) << "/" << m_steps.size() << "] " << m_steps[i] << std::endl;
            
            int code = glia::util::Shell::run(m_steps[i], true);
            if (code != 0) {
                return {glia::core::ExitCode::PartialSuccess, 
                        "Sequence halted at step " + std::to_string(i + 1), 
                        "Failed command: " + m_steps[i]};
            }
        }
        
        return {glia::core::ExitCode::Success, m_name + " sequence completed successfully"};
    }

private:
    std::string m_name;
    std::string m_desc;
    std::vector<std::string> m_steps;
};

}

#endif
