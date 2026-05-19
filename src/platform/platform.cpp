#include "platform.h"
#include <cstdlib>
#include <filesystem>
#include <unistd.h>

namespace glia::platform {
std::string Environment::getVar(const std::string& name) {
    char* val = std::getenv(name.c_str());
    return val ? std::string(val) : "";
}

bool Environment::isTerminal() {
    return isatty(STDOUT_FILENO);
}

std::string FileSystem::canonical(const std::string& path) {
    if (std::filesystem::exists(path)) return std::filesystem::canonical(path).string();
    return path;
}
}
