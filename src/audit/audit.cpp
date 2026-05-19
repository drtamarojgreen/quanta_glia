#include "audit.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

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

} // namespace glia::audit
