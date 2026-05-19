#include "EnhancementsClass.h"
#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "report/reporter.h"
#include "harvest/harvester.h"
#include "prune/pruner.h"
#include "config/config.h"
#include "util/fact_utils.h"
#include "audit/audit.h"
#include "safety/safety.h"
#include "trigger/trigger.h"

namespace fs = std::filesystem;

void reporting_enhancement_verification(const std::map<std::string, std::string>& facts) {
    fs::path repoPath = fs::temp_directory_path() / "sdd_report_emp";
    fs::create_directories(repoPath / "src");
    std::ofstream f(repoPath / "src/main.py"); f << "print('test')"; f.close();
    auto data = glia::report::Reporter::analyze(repoPath.string());
    std::cout << "report_src_files = " << data.numSrcFiles << std::endl;
    std::cout << "report_tech_stack = " << data.techStack << std::endl;
    std::cout << "report_timestamp_format = " << (data.timestamp.find('T') != std::string::npos ? 1 : 0) << std::endl;
    fs::remove_all(repoPath);
}

void harvester_enhancement_verification(const std::map<std::string, std::string>& facts) {
    fs::path repoPath = fs::temp_directory_path() / "sdd_harvest_emp";
    fs::create_directories(repoPath / "allowed"); fs::create_directories(repoPath / "forbidden");
    std::ofstream f1(repoPath / "allowed/README.md"); f1 << "line 1\nline 2\n"; f1.close();
    std::ofstream f2(repoPath / "forbidden/README.md"); f2 << "secret"; f2.close();
    glia::config::Config config;
    config.searchPaths = {"/allowed"}; config.maxDepth = 1; config.maxLinesPerFile = 1; config.targetTopics = {"README"};
    auto extracted = glia::harvest::Harvester::extractText(repoPath.string(), config);
    std::cout << "harvester_files_extracted = " << extracted.size() << std::endl;
    if (extracted.count("allowed/README.md")) {
        std::string s = extracted["allowed/README.md"];
        int lines = 0; std::stringstream ss(s); std::string l; while(std::getline(ss, l)) lines++;
        std::cout << "harvester_line_count = " << lines << std::endl;
    } else std::cout << "harvester_line_count = 0" << std::endl;
    std::cout << "harvester_forbidden_found = " << (extracted.count("forbidden/README.md") ? 1 : 0) << std::endl;
    fs::remove_all(repoPath);
}

void harvester_collision_verification(const std::map<std::string, std::string>& facts) {
    fs::path repoPath = fs::temp_directory_path() / "sdd_harvest_coll";
    fs::create_directories(repoPath / "a"); fs::create_directories(repoPath / "b");
    std::ofstream f1(repoPath / "a/README.md"); f1 << "content a"; f1.close();
    std::ofstream f2(repoPath / "b/README.md"); f2 << "content b"; f2.close();
    glia::config::Config config; config.targetTopics = {"README"};
    auto extracted = glia::harvest::Harvester::extractText(repoPath.string(), config);
    std::cout << "harvester_unique_paths = " << extracted.size() << std::endl;
    std::cout << "harvester_path_a_exists = " << (extracted.count("a/README.md") ? 1 : 0) << std::endl;
    std::cout << "harvester_path_b_exists = " << (extracted.count("b/README.md") ? 1 : 0) << std::endl;
    fs::remove_all(repoPath);
}

void pruner_enhancement_verification(const std::map<std::string, std::string>& facts) {
    fs::path kbPath = fs::temp_directory_path() / "sdd_kb_emp";
    fs::create_directories(kbPath / "repo_main"); fs::create_directories(kbPath / "repo_dep");
    std::ofstream f(kbPath / "repo_main/requirements.txt"); f << "repo_dep\n"; f.close();
    auto deps = glia::prune::Pruner::buildDependencyMap(kbPath.string());
    std::cout << "pruner_deps_count = " << deps.size() << std::endl;
    std::cout << "pruner_is_dep_protected = " << (deps.count("repo_dep") ? 1 : 0) << std::endl;
    fs::remove_all(kbPath);
}

void audit_verification() {
    glia::audit::Audit::log("TEST", "TARGET", "RATIONALE");
    auto results = glia::audit::Audit::search("TEST");
    std::cout << "audit_entry_found = " << (results.empty() ? 0 : 1) << std::endl;
    fs::remove("audit.ledger");
}

void safety_verification() {
    bool allowed = glia::safety::PolicyGate::allowAction("DELETE", "critical_repo");
    std::cout << "safety_policy_block = " << (allowed ? 0 : 1) << std::endl;
}

void qprocess_verification() {
    fs::path scriptPath = fs::temp_directory_path() / "dummy_script_emp.py";
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
