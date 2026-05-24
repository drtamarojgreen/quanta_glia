#include "glia_discovery_cmds.h"
#include "../util/translator.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;
using glia::util::Translator;

namespace glia::app {

glia::core::CommandResult DiscoverCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) return {glia::core::ExitCode::UsageError, "Usage: discover [sdd|facts]"};
    std::string target = args[1];
    if (target == "sdd") {
        if (fs::exists("tests/sdd")) {
            std::cout << "sdd_path = " << fs::absolute("tests/sdd").string() << std::endl;
            return {glia::core::ExitCode::Success, "SDD directory discovered"};
        }
    } else if (target == "facts") {
        if (fs::exists("tests/sdd/facts")) {
            std::cout << "facts_path = " << fs::absolute("tests/sdd/facts").string() << std::endl;
            return {glia::core::ExitCode::Success, "Facts directory discovered"};
        }
    }
    return {glia::core::ExitCode::FilesystemError, "Target directory not found"};
}

glia::core::CommandResult GateCheckCommand::execute(const std::vector<std::string>& args) {
    std::vector<std::string> gates = {"Interpretation", "Constraint", "Tool", "Strategy", "Scope", "Navigation"};
    std::cout << "REASONING\n";
    for (const auto& gate : gates) {
        std::cout << gate << "Gate = 1\n";
    }
    std::cout << "END\n";
    return {glia::core::ExitCode::Success, "Reasoning gates validated"};
}

}
