#pragma once
#include <filesystem>
#include <chrono>
#include <string> // Added for std::string

namespace fs = std::filesystem;

// Helper to find the latest created run directory
fs::path findLatestRunDirectory();

// Helper to create a test source directory with dummy files
void setupTestSrcDir(const std::string& testSrcPath, const std::string& mainContent, const std::string& makefileContent = "all:\n\t@echo Building\n");

// Helper to clean up a test source directory
void cleanupTestSrcDir(const std::string& testSrcPath);

// Helper to get the base directory for test run files
std::string getTestRunBaseDir();