#include "platform.h"
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <io.h>
#define STDOUT_FILENO 1
#define isatty _isatty
#else
#include <unistd.h>
#include <sys/resource.h>
#endif

namespace glia::platform {
std::string Environment::getVar(const std::string& name) {
    char* val = std::getenv(name.c_str());
    return val ? std::string(val) : "";
}

bool Environment::isTerminal() {
    return isatty(STDOUT_FILENO);
}

long Environment::getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024; // Bytes to KB
    }
    return 0;
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
#ifdef __APPLE__
        return usage.ru_maxrss / 1024; // Bytes to KB
#else
        return usage.ru_maxrss; // Already KB on Linux
#endif
    }
    return 0;
#endif
}

double Environment::getCpuUsage() {
#ifdef _WIN32
    FILETIME createTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER kernel, user;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;
        return (kernel.QuadPart + user.QuadPart) / 10000000.0; // convert 100-ns intervals to seconds
    }
    return 0.0;
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        double utime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
        double stime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;
        return utime + stime;
    }
    return 0.0;
#endif
}

std::string FileSystem::canonical(const std::string& path) {
    if (std::filesystem::exists(path)) return std::filesystem::canonical(path).string();
    return path;
}
}
