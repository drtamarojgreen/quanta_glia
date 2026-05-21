#include "app/rule_loader.h"
#include "app/sequence_command.h"
#include "app/shell_command.h"
#include "util/shell_utils.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

void test_rule_loader_basic() {
    std::cout << "Running test_rule_loader_basic...\n";
    std::string testXml = "test_rules.xml";
    std::ofstream out(testXml);
    out << "<GliaRules>\n"
        << "  <Command>\n"
        << "    <Name>test-cmd</Name>\n"
        << "    <Description>Test Desc</Description>\n"
        << "    <ShellTemplate>echo hello</ShellTemplate>\n"
        << "  </Command>\n"
        << "</GliaRules>\n";
    out.close();

    glia::app::CommandRegistry registry;
    glia::app::RuleLoader::load(testXml, registry);

    auto* cmd = registry.getCommand("test-cmd");
    assert(cmd != nullptr);
    assert(cmd->name() == "test-cmd");
    assert(cmd->description() == "Test Desc");

    std::filesystem::remove(testXml);
}

void test_rule_loader_sequence() {
    std::cout << "Running test_rule_loader_sequence...\n";
    std::string testXml = "test_seq_rules.xml";
    std::ofstream out(testXml);
    out << "<GliaRules>\n"
        << "  <Command>\n"
        << "    <Name>seq-cmd</Name>\n"
        << "    <Description>Seq Desc</Description>\n"
        << "    <Steps>\n"
        << "      <Step>echo step1</Step>\n"
        << "      <Step>echo step2</Step>\n"
        << "    </Steps>\n"
        << "  </Command>\n"
        << "</GliaRules>\n";
    out.close();

    glia::app::CommandRegistry registry;
    glia::app::RuleLoader::load(testXml, registry);

    auto* cmd = registry.getCommand("seq-cmd");
    assert(cmd != nullptr);
    assert(cmd->name() == "seq-cmd");

    // Execution test (silent)
    auto result = cmd->execute({});
    assert(result.code == glia::core::ExitCode::Success);

    std::filesystem::remove(testXml);
}

void test_sequence_command_failure() {
    std::cout << "Running test_sequence_command_failure...\n";
    std::vector<std::string> steps = {"echo start", "false", "echo end"};
    glia::app::SequenceCommand seq("fail-test", "Should fail at step 2", steps);
    
    auto result = seq.execute({});
    assert(result.code == glia::core::ExitCode::PartialSuccess);
    assert(result.userMessage.find("halted at step 2") != std::string::npos);
}

void test_shell_utils_query() {
    std::cout << "Running test_shell_utils_query...\n";
    std::string output = glia::util::Shell::query("echo 'test output'");
    // Trim output since echo adds newline
    if (!output.empty() && output.back() == '\n') output.pop_back();
    assert(output == "test output");
}

// In a real environment, we'd add these to test_main.cpp or a separate runner
void run_glia_arch_tests() {
    test_rule_loader_basic();
    test_rule_loader_sequence();
    test_sequence_command_failure();
    test_shell_utils_query();
}
