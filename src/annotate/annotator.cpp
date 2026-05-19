#include "annotator.h"
#include <filesystem>
#include <fstream>
#include <regex>

namespace fs = std::filesystem;

namespace glia::annotate {
Annotation Annotator::annotate(const std::string& repoPath) {
    Annotation ann;
    fs::path p(repoPath);
    ann.repoName = p.filename().string();

    if (fs::exists(p)) {
        for (auto const& dir_entry : fs::recursive_directory_iterator(p)) {
            if (!dir_entry.is_regular_file()) continue;
            std::string ext = dir_entry.path().extension().string();
            if (ext == ".md") {
                std::ifstream f(dir_entry.path());
                std::string line;
                while (std::getline(f, line)) {
                    if (line.size() > 2 && line[0] == '#' && line[1] == ' ') {
                        ann.headings.push_back(line.substr(2));
                    }
                    if (line.find("ethics") != std::string::npos) ann.tags.insert("ethics");
                    if (line.find("usage") != std::string::npos) ann.tags.insert("usage");
                    if (line.find("deprecated") != std::string::npos) ann.tags.insert("obsolete");
                }
            }
        }
    }
    return ann;
}
}
