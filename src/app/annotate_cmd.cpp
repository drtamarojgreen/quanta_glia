#include "annotate_cmd.h"
#include "../annotate/annotator.h"
#include "../config/config.h"
#include "../util/translator.h"
#include <iostream>
#include <filesystem>
namespace glia::app {
glia::core::CommandResult AnnotateCommand::execute(const std::vector<std::string>& args) {
    glia::config::Config config;
    config.load("config.yaml");
    if (std::filesystem::exists(config.knowledgeBase)) {
        for (auto const& dir_entry : std::filesystem::directory_iterator(config.knowledgeBase)) {
            if (!dir_entry.is_directory()) continue;
            glia::annotate::Annotator::annotate(dir_entry.path().string());
        }
    }
    return {glia::core::ExitCode::Success, glia::util::Translator::t("msg_done")};
}
}
