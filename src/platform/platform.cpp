#include "platform.h"
#include <cstdlib>
#include <filesystem>
#include <unistd.h>
#include <sys/resource.h>

namespace glia::platform {
std::string Environment::getVar(const std::string& name) {
    char* val = std::getenv(name.c_str());
    return val ? std::string(val) : "";
}

bool Environment::isTerminal() {
    return isatty(STDOUT_FILENO);
}

long Environment::getMemoryUsage() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
#ifdef __APPLE__
        return usage.ru_maxrss / 1024; // Bytes to KB
#else
        return usage.ru_maxrss; // Already KB on Linux
#endif
    }
    return 0;
}

double Environment::getCpuUsage() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        double utime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
        double stime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;
        return utime + stime;
    }
    return 0.0;
}

std::string FileSystem::canonical(const std::string& path) {
    if (std::filesystem::exists(path)) return std::filesystem::canonical(path).string();
    return path;
}
}
