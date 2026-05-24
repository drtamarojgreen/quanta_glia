#include "DynamicCommandClass.h"
#include <iostream>
#include "app/command_loader.h"
#include "app/command_factory.h"
#include "app/about.h"
#include <filesystem>
#include <fstream>
#include <memory>

void dynamic_command_verification() {
    // Use a static lambda or similar to avoid capture issues with -fanalyzer
    glia::app::CommandFactory::registerCreator("AboutCommand", []() -> std::unique_ptr<glia::app::Command> {
        return std::make_unique<glia::app::AboutCommand>();
    });

    // Create a temporary rule file
    std::string testRules = "test_rules_dynamic.xml";
    std::ofstream out(testRules);
    out << "<glia_rules>\n";
    out << "  <command name=\"test-cmd\" description=\"Test description\" type=\"internal\" target=\"AboutCommand\"/>\n";
    out << "</glia_rules>\n";
    out.close();

    auto metaList = glia::app::CommandLoader::loadFromXml(testRules);
    std::cout << "dynamic_cmds_loaded = " << metaList.size() << std::endl;
    if (!metaList.empty()) {
        std::cout << "dynamic_cmd_name = " << metaList[0].name << std::endl;
        auto cmd = glia::app::CommandFactory::create(metaList[0]);
        std::cout << "dynamic_cmd_created = " << (cmd != nullptr ? 1 : 0) << std::endl;
    }

    if (std::filesystem::exists(testRules)) {
        std::filesystem::remove(testRules);
    }
}
