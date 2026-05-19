#ifndef GLIA_APP_AUDIT_CMD_H
#define GLIA_APP_AUDIT_CMD_H
#include "command.h"
namespace glia::app {
class AuditCommand : public Command {
public:
    std::string name() const override { return "audit"; }
    std::string description() const override { return "Search and display audit ledger"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};
}
#endif
