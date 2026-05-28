#ifndef GLIA_PLATFORM_PLATFORM_H
#define GLIA_PLATFORM_PLATFORM_H
#include <string>

namespace glia::platform {
class Environment {
public:
    static std::string getVar(const std::string& name);
    static bool isTerminal();
    static long getMemoryUsage();
    static double getCpuUsage();
};

class FileSystem {
public:
    static std::string canonical(const std::string& path);
};
}
#endif
