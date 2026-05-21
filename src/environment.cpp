#include "environment.h"
#include <filesystem>
#include <cstdlib>
#include <chrono>

namespace fs = std::filesystem;

Environment::Environment(const std::string& tempBaseDir) : tempBaseDir(tempBaseDir) {
    createNewRunDirectory();
}

void Environment::createNewRunDirectory() {
    static int counter = 0;
    auto ts = std::chrono::system_clock::now().time_since_epoch().count();
    // Create the run directory within tempBaseDir
    fs::path newWorkingDir = fs::path(tempBaseDir) / ("run_" + std::to_string(ts) + "_" + std::to_string(counter++));
    fs::create_directories(newWorkingDir); // Use create_directories to ensure parent exists
    workingDir = newWorkingDir.string();
}

void Environment::copySource(const std::string& srcDir) {
    fs::copy(srcDir, workingDir,
             fs::copy_options::recursive |
             fs::copy_options::overwrite_existing);
}

int Environment::runCommand(const std::string& command) {
    std::string fullCommand = "cd " + workingDir + " && " + command;
    return std::system(fullCommand.c_str());
}
