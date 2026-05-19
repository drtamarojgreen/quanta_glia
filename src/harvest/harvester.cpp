#include "harvester.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

namespace glia::harvest {

std::string Harvester::cloneRepo(const std::string& url, const std::string& cacheDir) {
    fs::path u(url);
    std::string repoName = u.filename().string();
    if (repoName.size() > 4 && repoName.substr(repoName.size()-4) == ".git") {
        repoName = repoName.substr(0, repoName.size()-4);
    }

    fs::path dest = fs::path(cacheDir) / repoName;
    if (fs::exists(dest)) return dest.string();

    if (fs::exists(u)) {
        fs::copy(u, dest, fs::copy_options::recursive);
        return dest.string();
    }

    std::string cmd = "git clone --depth=1 " + url + " " + dest.string() + " > /dev/null 2>&1";
    if (std::system(cmd.c_str()) == 0) return dest.string();

    return "";
}

std::map<std::string, std::string> Harvester::extractText(const std::string& repoPath, const glia::config::Config& config) {
    std::map<std::string, std::string> extracted;
    fs::path baseRepo(repoPath);

    for (const auto& sp : config.searchPaths) {
        std::string relSp = sp;
        if (!relSp.empty() && relSp[0] == '/') relSp = relSp.substr(1);
        fs::path searchPath = baseRepo / relSp;

        if (!fs::exists(searchPath)) continue;

        for (auto const& dir_entry : fs::recursive_directory_iterator(searchPath)) {
            if (!dir_entry.is_regular_file()) continue;

            fs::path p = dir_entry.path();
            fs::path relP = fs::relative(p, baseRepo);

            // Depth check
            int depth = std::distance(relP.begin(), relP.end()) - 1;
            if (depth > config.maxDepth) continue;

            bool matches = false;
            std::string filename = p.filename().string();
            for (const auto& topic : config.targetTopics) {
                if (filename.find(topic) != std::string::npos) {
                    matches = true; break;
                }
            }

            if (matches) {
                std::ifstream file(p);
                if (file.is_open()) {
                    std::string content;
                    std::string line;
                    int count = 0;
                    while (std::getline(file, line) && (config.maxLinesPerFile == 0 || count < config.maxLinesPerFile)) {
                        content += line + "\n";
                        count++;
                    }
                    extracted[relP.string()] = content;
                }
            }
        }
    }
    return extracted;
}

bool Harvester::persist(const std::string& repoName, const std::map<std::string, std::string>& content, const std::string& kbPath) {
    fs::path baseKb = fs::path(kbPath) / repoName;
    for (const auto& [relPath, text] : content) {
        fs::path target = baseKb / relPath;
        fs::create_directories(target.parent_path());
        std::ofstream out(target);
        if (out.is_open()) {
            out << text;
        } else return false;
    }
    return true;
}

} // namespace glia::harvest
