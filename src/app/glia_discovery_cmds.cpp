#include "glia_discovery_cmds.h"
#include "../util/translator.h"
#include "../util/shell_utils.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>

namespace fs = std::filesystem;
using glia::util::Translator;
using glia::util::exec;

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
    bool interpretation = fs::exists("data/rules.xml");
    bool constraint = fs::exists("RESTRICTIONS.md") && fs::file_size("RESTRICTIONS.md") > 0;
    bool tool = exec("git --version").exitCode == 0 &&
                exec("cmake --version").exitCode == 0 &&
                exec("g++ --version").exitCode == 0;
    bool strategy = fs::exists("tests/sdd/sorrel_checkins.md");
    bool scope = fs::exists("workspace") && fs::is_directory("workspace");
    bool navigation = fs::exists(".git");

    std::cout << "REASONING\n";
    std::cout << "InterpretationGate = " << (interpretation ? 1 : 0) << "\n";
    std::cout << "ConstraintGate = " << (constraint ? 1 : 0) << "\n";
    std::cout << "ToolGate = " << (tool ? 1 : 0) << "\n";
    std::cout << "StrategyGate = " << (strategy ? 1 : 0) << "\n";
    std::cout << "ScopeGate = " << (scope ? 1 : 0) << "\n";
    std::cout << "NavigationGate = " << (navigation ? 1 : 0) << "\n";
    std::cout << "END\n";

    bool allPassed = interpretation && constraint && tool && strategy && scope && navigation;
    return {allPassed ? glia::core::ExitCode::Success : glia::core::ExitCode::InternalFailure,
            allPassed ? "Reasoning gates validated" : "One or more reasoning gates failed"};
}

}
