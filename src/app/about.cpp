#include "about.h"
#include <iostream>
namespace glia::app {
glia::core::CommandResult AboutCommand::execute(const std::vector<std::string>& args) {
    std::cout << "Glia Standalone Console Application\n"
              << "Version: 1.1.0\n"
              << "Standard: C++17\n"
              << "Platform: Portable\n";
    return {glia::core::ExitCode::Success, "About displayed"};
}
}
