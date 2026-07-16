#ifndef GLIA_UTIL_SHELL_UTILS_H
#define GLIA_UTIL_SHELL_UTILS_H
#include <string>
#include <vector>
#include <array>
#include <cstdio>
#include <stdexcept>

#ifdef _WIN32
#include <io.h>
#define popen _popen
#define pclose _pclose
#else
#include <sys/wait.h>
#endif

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
#ifdef _WIN32
    int exitCode = status;
#else
    int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
    return {result, exitCode};
}

}
#endif
