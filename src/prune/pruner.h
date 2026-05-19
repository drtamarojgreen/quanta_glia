#ifndef GLIA_PRUNE_PRUNER_H
#define GLIA_PRUNE_PRUNER_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <chrono>

namespace glia::prune {

struct RepoPruneRecord {
    std::string name;
    std::string path;
    std::chrono::system_clock::time_point lastActivity;
    float usageScore;
    float redundancyScore;
    float ethicsRiskScore;
};

class Pruner {
public:
    static std::set<std::string> buildDependencyMap(const std::string& kbPath);
    static float calculateScore(const RepoPruneRecord& repo, const std::map<std::string, float>& weights);
    static std::string makeDecision(float score, const std::map<std::string, float>& thresholds);
    static bool execute(const std::string& repoPath, const std::string& decision, const std::string& archivePath, bool dryRun);
};

} // namespace glia::prune

#endif
