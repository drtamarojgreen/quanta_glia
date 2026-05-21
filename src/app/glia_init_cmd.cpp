#include "glia_init_cmd.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace glia::app {

glia::core::CommandResult GliaInitCommand::execute(const std::vector<std::string>& args) {
    std::string buildDir = std::filesystem::current_path().string();
    if (buildDir.find("/build") == std::string::npos) {
        // Assume we are in the root if not in build
        buildDir += "/build";
    }
    
    char* home = std::getenv("HOME");
    if (!home) {
        return {glia::core::ExitCode::InternalFailure, "Could not find HOME directory"};
    }
    
    std::string bashrcPath = std::string(home) + "/.bashrc";
    std::ofstream bashrc(bashrcPath, std::ios::app);
    if (!bashrc) {
        return {glia::core::ExitCode::InternalFailure, "Could not open .bashrc for writing"};
    }
    
    bashrc << "\n# Glia Path\n";
    bashrc << "export PATH=\"$PATH:" << buildDir << "\"\n";
    bashrc.close();
    
    std::cout << "Added " << buildDir << " to PATH in .bashrc" << std::endl;
    std::cout << "Please run 'source ~/.bashrc' to apply changes." << std::endl;
    
    return {glia::core::ExitCode::Success, "Glia initialized and added to path"};
}

}
