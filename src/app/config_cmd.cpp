#include "config_cmd.h"
#include "../config/config.h"
#include "../util/translator.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult ConfigCommand::execute(const std::vector<std::string>& args) {
    return {glia::core::ExitCode::Success, glia::util::Translator::t("msg_done")};
}
}
