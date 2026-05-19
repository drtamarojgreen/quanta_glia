#include "harvest_cmd.h"
#include "../harvest/harvester.h"
#include "../config/config.h"
#include "../audit/audit.h"
#include <iostream>
#include <filesystem>

namespace glia::app {
glia::core::CommandResult HarvestCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) return {glia::core::ExitCode::UsageError, "No repos specified"};

    glia::config::Config config;
    config.load("config.yaml");

    for (size_t i = 1; i < args.size(); ++i) {
        const auto& url = args[i];
        std::cout << "--- Processing: " << url << " ---\n";
        std::string path = glia::harvest::Harvester::cloneRepo(url, config.repoCache);
        if (!path.empty()) {
            auto ext = glia::harvest::Harvester::extractText(path, config);
            if (glia::harvest::Harvester::persist(std::filesystem::path(path).filename().string(), ext, config.knowledgeBase)) {
                std::cout << "Stored " << ext.size() << " files\n";
                glia::audit::Audit::log("HARVEST", url, "Extracted " + std::to_string(ext.size()) + " files");
            }
        }
    }
    return {glia::core::ExitCode::Success, "Harvest complete"};
}
}
