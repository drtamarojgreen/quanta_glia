#include "reporter.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <map>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

namespace glia::report {

ReportData Reporter::analyze(const std::string& repoPath) {
    ReportData data;
    fs::path p(repoPath);
    data.repoName = p.filename().string();
    data.repoSize = 0;
    data.numFiles = 0;
    data.numSrcFiles = 0;
    data.numDocFiles = 0;
    data.numTestFiles = 0;
    data.techStack = "Unknown";

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%S") << "Z";
    data.timestamp = ss.str();

    std::map<std::string, int> extFreq;
    std::vector<std::string> srcExts = {".py", ".js", ".java", ".c", ".cpp", ".go", ".rs", ".ts", ".html", ".css"};
    std::vector<std::string> docExts = {".md", ".txt", ".rst"};

    if (fs::exists(p)) {
        for (auto const& dir_entry : fs::recursive_directory_iterator(p)) {
            if (dir_entry.path().string().find(".git") != std::string::npos) continue;
            if (!dir_entry.is_regular_file()) continue;

            data.numFiles++;
            data.repoSize += dir_entry.file_size();

            std::string ext = dir_entry.path().extension().string();
            std::string extLower = ext;
            std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);

            std::string stem = dir_entry.path().stem().string();
            std::string stemUpper = stem;
            std::transform(stemUpper.begin(), stemUpper.end(), stemUpper.begin(), ::toupper);

            std::string fullPathLower = dir_entry.path().string();
            std::transform(fullPathLower.begin(), fullPathLower.end(), fullPathLower.begin(), ::tolower);

            if (fullPathLower.find("test") != std::string::npos) {
                data.numTestFiles++;
            } else if (std::find(docExts.begin(), docExts.end(), extLower) != docExts.end() || stemUpper == "LICENSE" || stemUpper == "CONTRIBUTING") {
                data.numDocFiles++;
            } else if (std::find(srcExts.begin(), srcExts.end(), extLower) != srcExts.end()) {
                data.numSrcFiles++;
                extFreq[extLower]++;
            }
        }
    }

    if (!extFreq.empty()) {
        std::string best; int maxC = -1;
        for (auto const& [ext, count] : extFreq) {
            if (count > maxC) { maxC = count; best = ext; }
        }
        data.techStack = best;
    }

    return data;
}

bool Reporter::writeCsv(const ReportData& data, const std::string& filename) {
    bool exists = fs::exists(filename);
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) return false;

    if (!exists) {
        out << "repository_name,repository_size,number_files,number_src_files,number_doc_files,number_test_files,tech_stack,timestamp\n";
    }

    auto escape = [](std::string s) {
        if (s.find(',') != std::string::npos || s.find('"') != std::string::npos) {
            size_t pos = 0;
            while ((pos = s.find('"', pos)) != std::string::npos) { s.replace(pos, 1, "\"\""); pos += 2; }
            return "\"" + s + "\"";
        }
        return s;
    };

    out << escape(data.repoName) << "," << data.repoSize << "," << data.numFiles << ","
        << data.numSrcFiles << "," << data.numDocFiles << "," << data.numTestFiles << ","
        << escape(data.techStack) << "," << escape(data.timestamp) << "\n";
    return true;
}

} // namespace glia::report
