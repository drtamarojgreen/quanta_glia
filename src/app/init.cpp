#include "init.h"
#include "../storage/storage.h"
#include "../util/translator.h"
#include "../config/config.h"
namespace glia::app {
glia::core::CommandResult InitCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    glia::config::Config config;
    config.load("config.yaml");
    if (glia::storage::Storage::init(config.knowledgeBase, config.repoCache)) {
        return {glia::core::ExitCode::Success, Translator::t("msg_done")};
    }
    return {glia::core::ExitCode::FilesystemError, Translator::t("msg_error")};
}
}
