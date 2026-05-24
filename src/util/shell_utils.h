#ifndef GLIA_UTIL_SHELL_UTILS_H
#define GLIA_UTIL_SHELL_UTILS_H
<<<<<<< HEAD

#include <string>
#include <vector>
#include <memory>
#include <array>
#include <stdexcept>
#include <iostream>
#include "../core/result.h"

namespace glia::util {

class Shell {
public:
    /**
     * Executes a command and returns the output (stdout).
     * Throws std::runtime_error on pipe failure.
     */
    static std::string query(const std::string& cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    /**
     * Executes a command and returns the exit code.
     * Prints the command to stdout for transparency.
     */
    static int run(const std::string& cmd, bool silent = false) {
        if (!silent) std::cout << ">> " << cmd << std::endl;
        return std::system(cmd.c_str());
    }

    /**
     * Helper to wrap shell results into glia::core::CommandResult.
     */
    static glia::core::CommandResult makeResult(int exitCode, const std::string& successMsg, const std::string& failMsg) {
        if (exitCode == 0) {
            return {glia::core::ExitCode::Success, successMsg};
        }
        return {glia::core::ExitCode::InternalFailure, failMsg + " (Exit code: " + std::to_string(exitCode) + ")"};
    }
};

}

=======
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
>>>>>>> origin/glia-workflow-enhancement-15384845863160463952
#endif
