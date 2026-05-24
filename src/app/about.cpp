#include "about.h"
#include "../util/translator.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult AboutCommand::execute(const std::vector<std::string>& args) {
    std::cout << glia::util::Translator::t("app_name") << "\n";
    return {glia::core::ExitCode::Success, glia::util::Translator::t("msg_done")};
}
}
