#include "test_helpers.h"
#include <fstream> // Required for std::ofstream
#include "config.h" // Include config header
#include <iostream> // For std::cerr

// Helper to get the base directory for test run files
std::string getTestRunBaseDir() {
    std::string baseDir;
    try {
        Config testConfig = Config::load("config-test.yaml"); // Correct initialization
        baseDir = testConfig.getString("temp_base_dir");
    } catch (const std::runtime_error& e) {
        std::cerr << "Error loading config-test.yaml: " << e.what() << std::endl;
        baseDir = "test_runs"; // Fallback
    }

    if (!fs::exists(baseDir)) {
        fs::create_directories(baseDir);
    }
    return baseDir;
}

// Helper to find the latest created run directory
fs::path findLatestRunDirectory() {
    fs::path latestDir;
    fs::file_time_type latestTime;

    // Search within the test run base directory
    for (const auto& entry : fs::directory_iterator(getTestRunBaseDir())) {
        if (entry.is_directory() && entry.path().filename().string().rfind("run_", 0) == 0) {
            auto fileTime = fs::last_write_time(entry.path());
            if (latestDir.empty() || fileTime > latestTime) {
                latestTime = fileTime;
                latestDir = entry.path();
            }
        }
    }
    return latestDir;
}

void setupTestSrcDir(const std::string& testSrcPath, const std::string& mainContent, const std::string& makefileContent) {
    if (fs::exists(testSrcPath)) {
        fs::remove_all(testSrcPath);
    }
    fs::create_directories(testSrcPath); // Use create_directories for nested paths

    // Create a dummy main.cpp
    std::ofstream mainOut(testSrcPath + "/main.cpp");
    mainOut << mainContent;
    mainOut.close();

    // Create a dummy Makefile
    std::ofstream makeOut(testSrcPath + "/Makefile");
    makeOut << makefileContent;
    makeOut.close();
}

void cleanupTestSrcDir(const std::string& testSrcPath) {
    if (fs::exists(testSrcPath)) {
        fs::remove_all(testSrcPath);
    }
}