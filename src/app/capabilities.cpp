#include "capabilities.h"
#include "../util/translator.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult CapabilitiesCommand::execute(const std::vector<std::string>& args) {
    return {glia::core::ExitCode::Success, glia::util::Translator::t("msg_done")};
}
}
