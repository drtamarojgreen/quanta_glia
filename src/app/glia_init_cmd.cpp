#include "glia_init_cmd.h"
#include "default_rules.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace glia::app {

glia::core::CommandResult GliaInitCommand::execute(const std::vector<std::string>& args) {
    char* home = std::getenv("HOME");
    if (!home) {
        return {glia::core::ExitCode::InternalFailure, "Could not find HOME directory"};
    }
    std::string homeDir(home);
    std::string gliaDir = homeDir + "/.glia";
    
    // Create ~/.glia directory
    try {
        std::filesystem::create_directories(gliaDir);
        std::cout << "Created directory: " << gliaDir << std::endl;
    } catch (const std::exception& e) {
        return {glia::core::ExitCode::FilesystemError, "Failed to create ~/.glia: " + std::string(e.what())};
    }

    // Write rules files in ~/.glia dynamically from the binary
    auto writeEmbeddedFile = [&](const std::string& name, const std::string& content) {
        std::string targetPath = gliaDir + "/" + name;
        if (!std::filesystem::exists(targetPath)) {
            std::ofstream out(targetPath);
            if (out) {
                out << content;
                std::cout << "Initialized " << targetPath << " from binary resources." << std::endl;
            } else {
                std::cerr << "Error: Could not open " << targetPath << " for writing." << std::endl;
            }
        }
    };

    writeEmbeddedFile("rules.xml", DEFAULT_RULES_XML);
    writeEmbeddedFile("rules.xsd", DEFAULT_RULES_XSD);

    std::string buildDir = std::filesystem::current_path().string();
    if (buildDir.find("/build") == std::string::npos) {
        buildDir += "/build";
    }
    
    std::string bashrcPath = homeDir + "/.bashrc";
    std::ofstream bashrc(bashrcPath, std::ios::app);
    if (!bashrc) {
        return {glia::core::ExitCode::InternalFailure, "Could not open .bashrc for writing"};
    }
    
    bashrc << "\n# Glia Path\n";
    bashrc << "export PATH=\"$PATH:" << buildDir << "\"\n";
    bashrc.close();
    
    std::cout << "Added " << buildDir << " to PATH in .bashrc" << std::endl;
    std::cout << "Please run 'source ~/.bashrc' to apply changes." << std::endl;
    
    return {glia::core::ExitCode::Success, "Glia initialized globally at " + gliaDir};
}

}
