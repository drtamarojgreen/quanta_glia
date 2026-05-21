#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include "app/rule_loader.h"
#include "app/command.h"

void glia_automation_sdd_verification() {
    std::cout << "[SDD] Verifying Glia Automation Architecture..." << std::endl;
    
    // Test dynamic loading requirement from SDD
    glia::app::CommandRegistry registry;
    glia::app::RuleLoader::load("data/rules.xml", registry);
    
    auto commands = registry.listCommands();
    bool hasEnvRebuild = false;
    for (const auto& cmd : commands) {
        if (cmd == "env-rebuild") hasEnvRebuild = true;
    }
    
    if (hasEnvRebuild) {
        std::cout << "  - Requirement: Dynamic multi-step commands ... PASSED" << std::endl;
    } else {
        std::cout << "  - Requirement: Dynamic multi-step commands ... FAILED" << std::endl;
    }

    // Verify shell utility robustness
    try {
        std::string output = glia::util::Shell::query("echo 'glia-sdd'");
        if (output.find("glia-sdd") != std::string::npos) {
            std::cout << "  - Requirement: Robust shell execution ... PASSED" << std::endl;
        }
    } catch (...) {
        std::cout << "  - Requirement: Robust shell execution ... FAILED" << std::endl;
    }
}

// Note: This would normally be called from a main runner in the SDD suite
void run_glia_automation_card() {
    glia_automation_sdd_verification();
}
