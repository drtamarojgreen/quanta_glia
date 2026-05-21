#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include "environment.h"
#include "test_helpers.h" // Include test helpers

namespace fs = std::filesystem;

void test_environment_create_directory() {
    Environment env(getTestRunBaseDir()); // Pass tempBaseDir
    assert(!env.workingDir.empty());
    assert(fs::exists(env.workingDir));
    assert(fs::is_directory(env.workingDir));
}

void test_environment_copy_source() {
    // create a temporary source directory with dummy main.cpp and Makefile
    const std::string srcDir = "data/src/env_test_src"; // Use data/src for test source
    const std::string mainContent = "int main(){return 0;}\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    setupTestSrcDir(srcDir, mainContent, makefileContent); // Creates srcDir/main.cpp and srcDir/Makefile

    Environment env(getTestRunBaseDir()); // Pass tempBaseDir
    env.copySource(srcDir); // Copies srcDir to env.workingDir

    // Assert that main.cpp is copied and content is correct
    std::string copiedMainFile = env.workingDir + "/main.cpp";
    assert(fs::exists(copiedMainFile));
    std::ifstream mainIn(copiedMainFile);
    std::string content;
    std::getline(mainIn, content);
    assert(content == mainContent.substr(0, mainContent.length() - 1)); // Remove trailing newline for assert

    // Assert that Makefile is copied
    std::string copiedMakefile = env.workingDir + "/Makefile";
    assert(fs::exists(copiedMakefile));

    cleanupTestSrcDir(srcDir); // Clean up the test's source directory
}

void test_environment_run_command() {
    Environment env(getTestRunBaseDir()); // Pass tempBaseDir
    // We can't easily assert the effect portably, but we can at least call it.
    // Using a very simple command that should succeed on POSIX systems.
    // If you need full portability, you might skip this or adapt per platform.
    env.runCommand("echo EnvironmentTest");
}
