#ifndef GLIA_APP_GENOME_CMD_H
#define GLIA_APP_GENOME_CMD_H
#include "command.h"
namespace glia::app {
class GenomeCommand : public Command {
public:
    std::string name() const override { return m_meta.name; }
    std::string description() const override { return m_meta.description; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
