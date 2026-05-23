#include "glia_init_cmd.h"
#include "../util/translator.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace glia::app {

glia::core::CommandResult GliaInitCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    std::string buildDir = std::filesystem::current_path().string();
    if (buildDir.find("/build") == std::string::npos) buildDir += "/build";
    
    char* home = std::getenv("HOME");
    if (!home) return {glia::core::ExitCode::InternalFailure, Translator::t("msg_error")};
    
    std::string bashrcPath = std::string(home) + "/.bashrc";
    std::ofstream bashrc(bashrcPath, std::ios::app);
    if (!bashrc) return {glia::core::ExitCode::InternalFailure, Translator::t("msg_error")};
    
    bashrc << "\n# Glia\n" << "export PATH=\"$PATH:" << buildDir << "\"\n";
    bashrc.close();
    
    return {glia::core::ExitCode::Success, Translator::t("path_added")};
}

}
