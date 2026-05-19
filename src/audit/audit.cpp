#include "audit.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace glia::audit {

void Audit::log(const std::string& action, const std::string& target, const std::string& rationale) {
    std::ofstream out("audit.ledger", std::ios::app);
    if (out.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        out << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%S") << "Z | "
            << action << " | " << target << " | " << rationale << "\n";
    }
}

std::vector<AuditEntry> Audit::search(const std::string& query) {
    std::vector<AuditEntry> results;
    std::ifstream in("audit.ledger");
    std::string line;
    while (std::getline(in, line)) {
        if (line.find(query) != std::string::npos) {
            std::stringstream ss(line);
            std::string t, a, tr, r;
            std::getline(ss, t, '|');
            std::getline(ss, a, '|');
            std::getline(ss, tr, '|');
            std::getline(ss, r, '|');
            results.push_back({t, a, tr, r});
        }
    }
    return results;
}

} // namespace glia::audit
