#include "config_cmd.h"
#include "../config/config.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult ConfigCommand::execute(const std::vector<std::string>& args) {
    glia::config::Config config;
    config.load("config.yaml");
    if (args.size() > 1 && args[1] == "init") {
        std::cout << "Creating default config.yaml...\n";
        // Implementation for init...
    } else {
        std::cout << "Active Configuration:\n"
                  << "  knowledge_base: " << config.knowledgeBase << "\n"
                  << "  repo_cache:     " << config.repoCache << "\n"
                  << "  max_repos:      " << config.maxRepos << "\n"
                  << "  log_level:      " << config.logLevel << "\n";
    }
    return {glia::core::ExitCode::Success, "Config displayed"};
}
}
