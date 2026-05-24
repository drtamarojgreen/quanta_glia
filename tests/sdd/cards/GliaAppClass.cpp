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
#include "WorkflowClass.h"
#include "WasteClass.h"
#include "RobustnessClass.h"
#include "DynamicCommandClass.h"
#include "ScoringClass.h"

namespace fs = std::filesystem;

void sorrel_glia_config_serialization_card(const std::map<std::string, std::string>& facts) {
    fs::path p = fs::temp_directory_path() / "test_config_emp.txt";
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

int main(int argc, char** argv) {
    auto facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/environment.facts");
    auto enh_facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/enhancements.facts");
    auto audit_facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/glia_audit.facts");
    facts.insert(enh_facts.begin(), enh_facts.end());
    facts.insert(audit_facts.begin(), audit_facts.end());

    if (argc < 2) {
        sorrel_glia_config_serialization_card(facts);
        reporting_enhancement_verification(facts);
        harvester_enhancement_verification(facts);
        pruner_enhancement_verification(facts);
        harvester_collision_verification(facts);
        audit_verification();
        safety_verification();
        qprocess_verification();
        workflow_verification();
        waste_verification();
        robustness_verification();
        dynamic_command_verification();
        scoring_verification();
        return 0;
    }

    std::string cardName = argv[1];
    if (cardName == "config_serialization") sorrel_glia_config_serialization_card(facts);
    else if (cardName == "reporting") reporting_enhancement_verification(facts);
    else if (cardName == "harvester") harvester_enhancement_verification(facts);
    else if (cardName == "pruner") pruner_enhancement_verification(facts);
    else if (cardName == "harvester_collision") harvester_collision_verification(facts);
    else if (cardName == "audit") audit_verification();
    else if (cardName == "safety") safety_verification();
    else if (cardName == "qprocess") qprocess_verification();
    else if (cardName == "workflow") workflow_verification();
    else if (cardName == "waste") waste_verification();
    else if (cardName == "robustness") robustness_verification();
    else if (cardName == "dynamic_command") dynamic_command_verification();
    else if (cardName == "scoring") scoring_verification();
    else {
        std::cerr << "Unknown logical card: " << cardName << std::endl;
        return 1;
    }

    return 0;
}
