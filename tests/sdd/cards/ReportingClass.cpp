#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: reporting_csv_verification
// @Is python_available == 1
// @Results reporting_csv_operational == 1
void reporting_csv_verification_card(const std::map<std::string, std::string>& facts) {
    std::string test_repo = "test_reporting_repo";
    fs::create_directory(test_repo);
    {
        std::ofstream f(fs::path(test_repo) / "test.py");
        f << "print('hello world')\n";
    }

    std::string csv_file = "repository_data.csv";
    if (fs::exists(csv_file)) fs::remove(csv_file);

    std::string command = "python3 scripts/reporting.py " + test_repo + " > /dev/null 2>&1";
    int exit_code = std::system(command.c_str());

    bool exists = fs::exists(csv_file);
    int operational = (exit_code == 0 && exists) ? 1 : 0;

    std::cout << "reporting_csv_operational = " << operational << std::endl;

    // Cleanup
    fs::remove_all(test_repo);
    if (fs::exists(csv_file)) fs::remove(csv_file);
    if (fs::exists("repo_cache")) fs::remove_all("repo_cache");
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    reporting_csv_verification_card(facts);
    return 0;
}
