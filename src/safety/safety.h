#ifndef GLIA_SAFETY_SAFETY_H
#define GLIA_SAFETY_SAFETY_H
#include <string>
#include <vector>

namespace glia::safety {
class PolicyGate {
public:
    static bool allowAction(const std::string& action, const std::string& target);
};

class Sandbox {
public:
    static bool validatePath(const std::string& path);
};
}
#endif
