#include "annotate_cmd.h"
#include "../annotate/annotator.h"
#include "../config/config.h"
#include <iostream>
#include <filesystem>
namespace glia::app {
glia::core::CommandResult AnnotateCommand::execute(const std::vector<std::string>& args) {
    glia::config::Config config;
    config.load("config.yaml");
    std::cout << "Annotating Knowledge Base...\n";
    if (std::filesystem::exists(config.knowledgeBase)) {
        for (auto const& dir_entry : std::filesystem::directory_iterator(config.knowledgeBase)) {
            if (!dir_entry.is_directory()) continue;
            auto ann = glia::annotate::Annotator::annotate(dir_entry.path().string());
            std::cout << "  Repo: " << ann.repoName << " Tags: ";
            for (const auto& tag : ann.tags) std::cout << tag << " ";
            std::cout << "\n";
        }
    }
    return {glia::core::ExitCode::Success, "Annotation complete"};
}
}
