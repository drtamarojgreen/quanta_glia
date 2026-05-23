#ifndef GLIA_CONFIG_CONFIG_H
#define GLIA_CONFIG_CONFIG_H

#include <string>
#include <vector>
#include <map>

namespace glia::config {

class Config {
public:
    Config();
    bool load(const std::string& path);

    std::string knowledgeBase;
    std::string repoCache;
    std::string rulesPath;
    std::string statePath;
    int maxRepos;
    std::string logLevel;
    std::vector<std::string> searchPaths;
    int maxDepth;
    int maxLinesPerFile;
    std::vector<std::string> targetTopics;
};

} // namespace glia::config

#endif
