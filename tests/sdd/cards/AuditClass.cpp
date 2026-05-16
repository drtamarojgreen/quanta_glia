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
    fs::path temp_root("tests/temp");
    std::string log_file = "audit.log";
    fs::path log_path = temp_root / log_file;
    if (fs::exists(log_path)) fs::remove(log_path);

    // audit.py uses a hardcoded AUDIT_LOG_FILE = 'audit.log' in the current working directory.
    std::string python_cmd = "cd tests/temp && python3 -c \"import sys; sys.path.insert(0, '../../'); from scripts.audit import log_audit_event; log_audit_event('test_event', {'info': 'test_details'})\"";
    int exit_code = std::system(python_cmd.c_str());

    long long log_size = 0;
    int event_found = 0;

    if (exit_code == 0 && fs::exists(log_path)) {
        log_size = fs::file_size(log_path);
        std::ifstream f(log_path);
        std::string line;
        if (std::getline(f, line)) {
            if (line.find("test_event") != std::string::npos && line.find("test_details") != std::string::npos) {
                event_found = 1;
            }
        }
    }

    std::cout << "audit_log_size_bytes = " << log_size << std::endl;
    std::cout << "audit_event_found = " << event_found << std::endl;

    if (fs::exists(log_path)) fs::remove(log_path);
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    audit_log_verification_card(facts);
    return 0;
}
