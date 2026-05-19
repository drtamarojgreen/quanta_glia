#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "config/config.h"
#include "harvest/harvester.h"
#include "prune/pruner.h"
#include "report/reporter.h"
#include "audit/audit.h"
#include "cli/cli.h"
#include "storage/storage.h"

namespace fs = std::filesystem;

void print_help() {
    std::cout << "Glia Standalone Console Application (SDD Phase III)\n"
              << "Usage: sorrel [options] [repos...]\n"
              << "Actions:\n"
              << "  -a harvest    Clone and extract information from repos\n"
              << "  -a prune      Evaluate and prune the knowledge base\n"
              << "  -a report     Generate analytics report\n"
              << "Options:\n"
              << "  -c <file>     Path to config file\n"
              << "  --dry-run     Simulate destructive actions\n"
              << "  --strategy <s|a> Strategy (conservative, aggressive)\n";
}

int main(int argc, char** argv) {
    glia::cli::ArgumentParser parser;
    parser.addOption("-a", "action");
    parser.addOption("-c", "config");
    parser.parse(argc, argv);

    if (parser.hasFlag("-h") || parser.hasFlag("--help") || argc == 1) {
        print_help(); return 0;
    }

    std::string action = parser.getOption("-a");
    std::string configPath = parser.getOption("-c");
    if (configPath.empty()) configPath = "config.yaml";

    bool dryRun = parser.hasFlag("--dry-run");
    std::string strategy = parser.getOption("--strategy");
    if (strategy.empty()) strategy = "conservative";

    std::vector<std::string> repos = parser.getPositional();

    glia::config::Config config;
    config.load(configPath);
    glia::storage::Storage::init(config.knowledgeBase, config.repoCache);

    if (action == "harvest") {
        if (repos.empty()) { std::cerr << "Error: No repos specified\n"; return 1; }
        for (const auto& url : repos) {
            std::cout << "--- Processing: " << url << " ---\n";
            std::string path = glia::harvest::Harvester::cloneRepo(url, config.repoCache);
            if (!path.isEmpty()) {
                auto ext = glia::harvest::Harvester::extractText(path, config);
                if (glia::harvest::Harvester::persist(fs::path(path).filename().string(), ext, config.knowledgeBase)) {
                    std::cout << "Stored " << ext.size() << " files\n";
                    glia::audit::Audit::log("HARVEST", url, "Extracted " + std::to_string(ext.size()) + " files");
                }
            }
        }
    } else if (action == "prune") {
        std::cout << "Starting Pruner...\n";
        auto deps = glia::prune::Pruner::buildDependencyMap(config.knowledgeBase);
        if (fs::exists(config.knowledgeBase)) {
            for (auto const& dir_entry : fs::directory_iterator(config.knowledgeBase)) {
                if (!dir_entry.is_directory()) continue;
                std::string name = dir_entry.path().filename().string();
                if (deps.count(name)) {
                    std::cout << "SKIPPING " << name << " (Active Dependency)\n";
                    continue;
                }

                glia::prune::RepoPruneRecord rec;
                rec.name = name;
                rec.path = dir_entry.path().string();
                rec.lastActivity = std::chrono::system_clock::from_time_t(fs::last_write_time(dir_entry).time_since_epoch().count());
                rec.usageScore = 0.0f; rec.redundancyScore = 0.0f; rec.ethicsRiskScore = 0.0f;

                float score = glia::prune::Pruner::calculateScore(rec, {{"usage", 0.5f}, {"age", 0.2f}});
                std::string decision = glia::prune::Pruner::makeDecision(score, {{"delete", 0.9f}, {"archive", 0.7f}});

                if (decision == "DELETE" && strategy != "aggressive") decision = "ARCHIVE";

                std::cout << "Evaluated " << name << ": " << decision << " (Score: " << score << ")\n";
                glia::prune::Pruner::execute(rec.path, decision, "repo_archive", dryRun);
                glia::audit::Audit::log("PRUNE", name, "Decision: " + decision + ", Score: " + std::to_string(score));
            }
        }
    } else if (action == "report") {
        std::cout << "Generating Report...\n";
        if (fs::exists(config.knowledgeBase)) {
            for (auto const& dir_entry : fs::directory_iterator(config.knowledgeBase)) {
                if (!dir_entry.is_directory()) continue;
                auto data = glia::report::Reporter::analyze(dir_entry.path().string());
                if (glia::report::Reporter::writeCsv(data, "repository_data.csv")) {
                    std::cout << "Reported " << data.repoName << "\n";
                }
            }
        }
    } else {
        print_help();
    }
    return 0;
}
