#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: audit_log_verification
// @Is python_available == 1
// @Results audit_log_operational == 1
void audit_log_verification_card(const std::map<std::string, std::string>& facts) {
    std::string log_file = "audit.log";
    if (fs::exists(log_file)) fs::remove(log_file);

    std::string python_cmd = "python3 -c \"from scripts.audit import log_audit_event; log_audit_event('test_event', {'info': 'test_details'})\"";
    int exit_code = std::system(python_cmd.c_str());

    bool operational = false;
    if (exit_code == 0 && fs::exists(log_file)) {
        std::ifstream f(log_file);
        std::string line;
        if (std::getline(f, line)) {
            if (line.find("test_event") != std::string::npos && line.find("test_details") != std::string::npos) {
                operational = true;
            }
        }
    }

    std::cout << "audit_log_operational = " << (operational ? 1 : 0) << std::endl;

    if (fs::exists(log_file)) fs::remove(log_file);
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    audit_log_verification_card(facts);
    return 0;
}
