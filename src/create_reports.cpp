#include "create_reports.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void CreateReports::appendReport(const std::string& csvPath, const Alternative& alt, bool success) {
    bool exists = fs::exists(csvPath);
    
    // Ensure directory exists
    fs::path p(csvPath);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }

    std::ofstream out(csvPath, std::ios::app);
    if (!exists) {
        out << "ErrorCode,Name,Confidence,Filename,StartLine,EndLine,Success\n";
    }
    
    out << alt.type << "," 
        << "\"" << alt.name << "\"," 
        << alt.confidence << ","
        << alt.filename << "," 
        << alt.startLine << "," 
        << alt.endLine << "," 
        << (success ? "true" : "false") << "\n";
    out.close();
}
