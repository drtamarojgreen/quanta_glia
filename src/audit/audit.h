#ifndef GLIA_AUDIT_AUDIT_H
#define GLIA_AUDIT_AUDIT_H

#include <string>
#include <fstream>

namespace glia::audit {

class Audit {
public:
    static void log(const std::string& action, const std::string& target, const std::string& rationale);
};

} // namespace glia::audit

#endif
