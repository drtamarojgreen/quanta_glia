#include "harvest_cmd.h"
#include "../harvest/harvester.h"
#include "../config/config.h"
#include "../audit/audit.h"
#include "../util/translator.h"
#include <iostream>
#include <filesystem>

namespace glia::app {
glia::core::CommandResult HarvestCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    if (args.size() < 2) return {glia::core::ExitCode::UsageError, Translator::t("msg_error")};

    glia::config::Config config;
    config.load("config.yaml");

    for (size_t i = 1; i < args.size(); ++i) {
        const auto& url = args[i];
        std::cout << Translator::t("harvest_start") << ": " << url << "\n";
        std::string path = glia::harvest::Harvester::cloneRepo(url, config.repoCache);
        if (!path.empty()) {
            auto ext = glia::harvest::Harvester::extractText(path, config);
            if (glia::harvest::Harvester::persist(std::filesystem::path(path).filename().string(), ext, config.knowledgeBase)) {
                glia::audit::Audit::log("HARVEST", url, Translator::t("msg_done"));
            }
        }
    }
    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
