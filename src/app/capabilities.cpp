#include "capabilities.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult CapabilitiesCommand::execute(const std::vector<std::string>& args) {
    std::cout << "Glia Capabilities:\n"
              << " - harvest: ENABLED\n"
              << " - prune: ENABLED\n"
              << " - report: ENABLED\n"
              << " - annotate: PLANNED\n"
              << " - curate: PLANNED\n";
    return {glia::core::ExitCode::Success, "Capabilities listed"};
}
}
