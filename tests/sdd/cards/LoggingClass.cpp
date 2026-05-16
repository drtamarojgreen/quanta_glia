#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: logging_setup_verification
// @Is python_available == true
// @Results logging_setup_operational == true
void logging_setup_verification_card(const std::map<std::string, std::string>& facts) {
    fs::path temp_root("tests/temp");
    std::string log_file_name = facts.at("log_file_path");
    fs::path log_file = temp_root / log_file_name;
    std::string message = facts.at("test_message");

    // Ensure the log file is in a directory os.path.dirname can handle
    std::string absolute_log_file = fs::absolute(log_file).string();

    // We use a small python snippet to trigger the setup_logging and log a message
    std::string python_cmd = "python3 -c \"from agent_utils.logging_utils import setup_logging; import logging; setup_logging('" + absolute_log_file + "'); logging.info('" + message + "')\"";

    std::system(python_cmd.c_str());

    long long log_size = 0;
    if (fs::exists(log_file)) {
        log_size = fs::file_size(log_file);
        fs::remove(log_file);
    }

    std::cout << "logging_setup_size_bytes = " << log_size << std::endl;
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/agent_utils.facts");
    if (facts.empty()) {
        std::cerr << "Error: Could not read facts from tests/sdd/facts/agent_utils.facts" << std::endl;
        return 1;
    }
    logging_setup_verification_card(facts);
    return 0;
}
