#include <iostream>
#include <cassert>
#include <filesystem>
#include "app/command_loader.h"
#include "util/fs_utils.h"

namespace fs = std::filesystem;

void test_rules_discovery() {
    std::cout << "Running test_rules_discovery..." << std::endl;
    std::string path = glia::util::findRulesXml();
    std::cout << "Found path: " << path << std::endl;
    assert(!path.empty());
    assert(fs::exists(path));
    assert(path.find("data/rules.xml") != std::string::npos);
    std::cout << "test_rules_discovery passed." << std::endl;
}

void test_rules_loading() {
    std::cout << "Running test_rules_loading..." << std::endl;
    std::string path = glia::util::findRulesXml();
    auto cmds = glia::app::CommandLoader::loadFromXml(path);
    assert(!cmds.empty());

    bool foundQtl = false;
    for (const auto& cmd : cmds) {
        if (cmd.name == "qtl") {
            foundQtl = true;
            break;
        }
    }
    assert(foundQtl);
    std::cout << "test_rules_loading passed." << std::endl;
}

int main() {
    test_rules_discovery();
    test_rules_loading();
    return 0;
}
