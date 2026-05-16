#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: io_save_load_verification
// @Is python_available == 1
// @Results io_save_load_operational == 1
void io_save_load_verification_card(const std::map<std::string, std::string>& facts) {
    fs::path temp_root("tests/temp");
    std::string test_dir_name = "test_io_dir";
    fs::path test_dir = temp_root / test_dir_name;
    std::string test_file = "test_io.txt";
    std::string content = "IO test content";

    std::string python_cmd = "python3 -c \"from agent_utils.io import save_output, load_content; save_output('" + content + "', '" + test_file + "', '" + test_dir.string() + "'); loaded = load_content('" + (test_dir / test_file).string() + "'); print(loaded == '" + content + "')\"";

    FILE* pipe = popen(python_cmd.c_str(), "r");
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    long long saved_file_size = 0;
    if (result.find("True") != std::string::npos) {
        fs::path saved_file = test_dir / test_file;
        if (fs::exists(saved_file)) {
            saved_file_size = fs::file_size(saved_file);
        }
    }
    std::cout << "saved_file_size_bytes = " << saved_file_size << std::endl;

    fs::remove_all(test_dir);
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    io_save_load_verification_card(facts);
    return 0;
}
