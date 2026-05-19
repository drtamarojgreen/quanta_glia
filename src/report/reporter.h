#ifndef GLIA_REPORT_REPORTER_H
#define GLIA_REPORT_REPORTER_H

#include <string>
#include <vector>
#include <map>

namespace glia::report {

struct ReportData {
    std::string repoName;
    long long repoSize;
    int numFiles;
    int numSrcFiles;
    int numDocFiles;
    int numTestFiles;
    std::string techStack;
    std::string timestamp;
};

class Reporter {
public:
    static ReportData analyze(const std::string& repoPath);
    static bool writeCsv(const ReportData& data, const std::string& filename);
};

} // namespace glia::report

#endif
