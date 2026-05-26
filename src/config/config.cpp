#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "../util/string_utils.h"

namespace glia::config {

Config::Config()
    : knowledgeBase("./knowledge_base")
    , repoCache("./repo_cache")
    , rulesPath("rules/rules.xml")
    , statePath("state.json")
    , maxRepos(10)
    , logLevel("INFO")
    , maxDepth(5)
    , maxLinesPerFile(500)
{
    targetTopics = {"README", "LICENSE", "CONTRIBUTING"};
    searchPaths = {"/"};
}

bool Config::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = glia::util::trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t sepPos = line.find('=');
        if (sepPos == std::string::npos) sepPos = line.find(':');

        if (sepPos != std::string::npos) {
            std::string key = glia::util::trim(line.substr(0, sepPos));
            std::string value = glia::util::trim(line.substr(sepPos + 1));

            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') || (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "knowledge_base") knowledgeBase = value;
            else if (key == "repo_cache") repoCache = value;
            else if (key == "rules_path") rulesPath = value;
            else if (key == "state_path") statePath = value;
            else if (key == "max_repos") maxRepos = std::stoi(value);
            else if (key == "log_level") logLevel = value;
            else if (key == "max_depth") maxDepth = std::stoi(value);
            else if (key == "max_lines_per_file") maxLinesPerFile = std::stoi(value);
            else if (key == "target_topics") {
                targetTopics.clear();
                std::stringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) targetTopics.push_back(glia::util::trim(item));
            }
            else if (key == "search_paths") {
                searchPaths.clear();
                std::stringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) searchPaths.push_back(glia::util::trim(item));
            }
        }
    }
    return true;
}

} // namespace glia::config
