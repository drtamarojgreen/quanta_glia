#pragma once
#include <string>

class Environment {
public:
    std::string workingDir;
    std::string tempBaseDir; // Base directory for run_XXXXX folders

    Environment(const std::string& tempBaseDir); // Constructor now takes tempBaseDir
    void createNewRunDirectory();
    void copySource(const std::string& srcDir);
    int runCommand(const std::string& command);
};
