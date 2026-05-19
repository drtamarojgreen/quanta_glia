#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "util/fact_utils.h"
#include "config/config.h"
#include "report/reporter.h"
#include "harvest/harvester.h"
#include "prune/pruner.h"
#include "EnhancementsClass.h"

namespace fs = std::filesystem;

void sorrel_glia_config_serialization_card(const std::map<std::string, std::string>& facts) {
    glia::config::Config config;
    config.knowledgeBase = "/tmp/kb";
    config.maxRepos = 42;
    config.logLevel = "DEBUG";

    fs::path p = fs::temp_directory_path() / "test_config_empirical.txt";
    std::ofstream out(p);
    out << "knowledge_base = /tmp/kb\nmax_repos = 42\nlog_level = DEBUG\n";
    out.close();

    glia::config::Config load;
    load.load(p.string());

    std::cout << "config_xml_max_repos = " << load.maxRepos << std::endl;
    std::cout << "config_xml_kb = " << load.knowledgeBase << std::endl;
    std::cout << "config_xml_log_level = " << load.logLevel << std::endl;

    fs::remove(p);
}

void sorrel_sdd_qprocess_integration_card(const std::map<std::string, std::string>& facts) {
    fs::path scriptPath = fs::temp_directory_path() / "dummy_script_empirical.py";
    std::ofstream file(scriptPath);
    file << "print('empirical_success')\n";
    file.close();

    std::string cmd = "python3 " + scriptPath.string();
    std::string output;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) output += buffer;
        pclose(pipe);
    }

    if (!output.empty() && output.back() == '\n') output.pop_back();

    std::cout << "qprocess_output = " << output << std::endl;
    fs::remove(scriptPath);
}

int main(int argc, char** argv) {
    auto facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/environment.facts");
    auto enh_facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/enhancements.facts");
    facts.insert(enh_facts.begin(), enh_facts.end());

    sorrel_glia_config_serialization_card(facts);
    sorrel_sdd_qprocess_integration_card(facts);
    reporting_enhancement_verification(facts);
    harvester_enhancement_verification(facts);
    pruner_enhancement_verification(facts);
    harvester_collision_verification(facts);

    return 0;
}
