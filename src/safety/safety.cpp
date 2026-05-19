#include "safety.h"
#include <filesystem>

namespace glia::safety {
bool PolicyGate::allowAction(const std::string& action, const std::string& target) {
    if (action == "DELETE" && target.find("critical") != std::string::npos) return false;
    return true;
}

bool Sandbox::validatePath(const std::string& path) {
    std::filesystem::path p(path);
    return p.is_relative() || p.string().find("knowledge_base") != std::string::npos;
}
}
