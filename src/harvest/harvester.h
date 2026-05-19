#ifndef GLIA_HARVEST_HARVESTER_H
#define GLIA_HARVEST_HARVESTER_H

#include <string>
#include <vector>
#include <map>
#include "../config/config.h"

namespace glia::harvest {

class Harvester {
public:
    static std::string cloneRepo(const std::string& url, const std::string& cacheDir);
    static std::map<std::string, std::string> extractText(const std::string& repoPath, const glia::config::Config& config);
    static bool persist(const std::string& repoName, const std::map<std::string, std::string>& content, const std::string& kbPath);
};

} // namespace glia::harvest

#endif
