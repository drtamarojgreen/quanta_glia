#ifndef GLIA_AUDIT_AUDIT_H
#define GLIA_AUDIT_AUDIT_H

#include <string>
#include <vector>

namespace glia::audit {

struct AuditEntry {
    std::string timestamp;
    std::string action;
    std::string target;
    std::string rationale;
};

class Audit {
public:
    static void log(const std::string& action, const std::string& target, const std::string& rationale);
    static std::vector<AuditEntry> search(const std::string& query);
};

} // namespace glia::audit

#endif
