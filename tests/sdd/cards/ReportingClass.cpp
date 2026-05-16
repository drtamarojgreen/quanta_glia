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
    fs::path temp_root("tests/temp");
    std::string test_repo_name = "test_reporting_repo";
    fs::path test_repo = temp_root / test_repo_name;
    fs::create_directory(test_repo);
    {
        std::ofstream f(test_repo / "test.py");
        f << "print('hello world')\n";
    }

    std::string csv_file = "repository_data.csv";
    if (fs::exists(csv_file)) fs::remove(csv_file);

    // Reporting.py currently doesn't take output path as argument, it uses hardcoded filename in current dir.
    // We should probably run it from tests/temp if possible, or just accept we have to clean up.
    std::string command = "cd tests/temp && python3 ../../scripts/reporting.py " + test_repo_name + " > /dev/null 2>&1";
    int exit_code = std::system(command.c_str());

    fs::path csv_path = temp_root / csv_file;

    long long csv_size = 0;
    int csv_row_count = 0;

    if (exit_code == 0 && fs::exists(csv_path)) {
        csv_size = fs::file_size(csv_path);
        std::ifstream f(csv_path);
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) csv_row_count++;
        }
    }

    std::cout << "csv_size_bytes = " << csv_size << std::endl;
    std::cout << "csv_row_count = " << csv_row_count << std::endl;

    // Cleanup
    fs::remove_all(test_repo);
    if (fs::exists(csv_path)) fs::remove(csv_path);
    if (fs::exists("repo_cache")) fs::remove_all("repo_cache");
    if (fs::exists(temp_root / "repo_cache")) fs::remove_all(temp_root / "repo_cache");
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    reporting_csv_verification_card(facts);
    return 0;
}
