#ifndef GLIA_UTIL_SHELL_UTILS_H
#define GLIA_UTIL_SHELL_UTILS_H
#include <string>
#include <vector>
#include <array>
#include <cstdio>
#include <stdexcept>
#include <sys/wait.h>

namespace glia::util {

struct ExecResult {
    std::string output;
    int exitCode;
};

inline ExecResult exec(const char* cmd, const std::string& cwd = "") {
    std::array<char, 128> buffer;
    std::string result;
    std::string fullCmd = cmd;
    if (!cwd.empty()) fullCmd = "cd " + cwd + " && " + fullCmd;
    FILE* pipe = popen(fullCmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) result += buffer.data();
    int status = pclose(pipe);
    int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    return {result, exitCode};
}

}
#endif
