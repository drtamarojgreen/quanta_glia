#include "pruner.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace glia::prune {

std::set<std::string> Pruner::buildDependencyMap(const std::string& kbPath) {
    std::set<std::string> deps;
    std::vector<std::string> manifests = {"requirements.txt", "package.json", "go.mod", "pom.xml"};

    std::vector<std::string> repoNames;
    if (fs::exists(kbPath)) {
        for (auto const& dir_entry : fs::directory_iterator(kbPath)) {
            if (dir_entry.is_directory()) repoNames.push_back(dir_entry.path().filename().string());
        }

        for (auto const& dir_entry : fs::recursive_directory_iterator(kbPath)) {
            if (!dir_entry.is_regular_file()) continue;
            std::string filename = dir_entry.path().filename().string();
            bool isManifest = false;
            for (const auto& m : manifests) if (filename == m) { isManifest = true; break; }

            if (isManifest) {
                std::ifstream f(dir_entry.path());
                if (f.is_open()) {
                    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                    for (const auto& name : repoNames) {
                        if (content.find(name) != std::string::npos) deps.insert(name);
                    }
                }
            }
        }
    }
    return deps;
}

float Pruner::calculateScore(const RepoPruneRecord& repo, const std::map<std::string, float>& weights) {
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - repo.lastActivity).count() / 24;
    float normalizedAge = std::min((float)age / 365.0f, 1.0f);

    float uw = weights.count("usage") ? weights.at("usage") : 0.5f;
    float aw = weights.count("age") ? weights.at("age") : 0.2f;
    float rw = weights.count("redundancy") ? weights.at("redundancy") : 0.2f;
    float ew = weights.count("ethics") ? weights.at("ethics") : 0.1f;

    return (1.0f - repo.usageScore) * uw + normalizedAge * aw + repo.redundancyScore * rw + repo.ethicsRiskScore * ew;
}

std::string Pruner::makeDecision(float score, const std::map<std::string, float>& thresholds) {
    float dt = thresholds.count("delete") ? thresholds.at("delete") : 0.9f;
    float at = thresholds.count("archive") ? thresholds.at("archive") : 0.7f;
    float rt = thresholds.count("review") ? thresholds.at("review") : 0.5f;

    if (score >= dt) return "DELETE";
    if (score >= at) return "ARCHIVE";
    if (score >= rt) return "REVIEW";
    return "KEEP";
}

bool Pruner::execute(const std::string& repoPath, const std::string& decision, const std::string& archivePath, bool dryRun) {
    if (decision == "KEEP" || decision == "REVIEW") return true;
    fs::path p(repoPath);
    if (decision == "ARCHIVE") {
        fs::path dest = fs::path(archivePath) / p.filename();
        fs::create_directories(archivePath);
        if (dryRun) { std::cout << "[DRY RUN] Archiving " << p.filename() << " to " << dest << std::endl; return true; }
        fs::rename(p, dest);
        return true;
    }
    if (decision == "DELETE") {
        if (dryRun) { std::cout << "[DRY RUN] Deleting " << p.filename() << std::endl; return true; }
        fs::remove_all(p);
        return true;
    }
    return false;
}

} // namespace glia::prune
