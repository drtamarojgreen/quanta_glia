#ifndef GLIA_APP_ANNOTATE_CMD_H
#define GLIA_APP_ANNOTATE_CMD_H
#include "command.h"
namespace glia::app {
class AnnotateCommand : public Command {
public:
    std::string name() const override { return "annotate"; }
    std::string description() const override { return "Generate semantic tags for records"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
