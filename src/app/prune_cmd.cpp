#include "prune_cmd.h"
#include "../prune/pruner.h"
#include "../config/config.h"
#include "../audit/audit.h"
#include "../util/translator.h"
#include <iostream>
#include <filesystem>

namespace glia::app {
glia::core::CommandResult PruneCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    glia::config::Config config;
    config.load("config.yaml");

    bool dryRun = false;
    for (const auto& arg : args) if (arg == "--dry-run") dryRun = true;

    std::string archiveDir = m_meta.params.count("archive_dir") ? m_meta.params.at("archive_dir") : "repo_archive";

    auto deps = glia::prune::Pruner::buildDependencyMap(config.knowledgeBase);
    if (std::filesystem::exists(config.knowledgeBase)) {
        for (auto const& dir_entry : std::filesystem::directory_iterator(config.knowledgeBase)) {
            if (!dir_entry.is_directory()) continue;
            std::string name = dir_entry.path().filename().string();
            if (deps.count(name)) continue;

            glia::prune::RepoPruneRecord rec;
            rec.name = name;
            rec.path = dir_entry.path().string();
            rec.lastActivity = std::chrono::system_clock::from_time_t(std::filesystem::last_write_time(dir_entry).time_since_epoch().count());
            rec.usageScore = 0.0f; rec.redundancyScore = 0.0f; rec.ethicsRiskScore = 0.0f;

            float score = glia::prune::Pruner::calculateScore(rec, {{"usage", 0.5f}, {"age", 0.2f}});
            std::string decision = glia::prune::Pruner::makeDecision(score, {{"delete", 0.9f}, {"archive", 0.7f}});

            glia::prune::Pruner::execute(rec.path, decision, archiveDir, dryRun);
            glia::audit::Audit::log("PRUNE", name, decision);
        }
    }
    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
