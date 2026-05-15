#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include <cstdio>
#include <array>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: ethos_validation_verification
// @Is python_available == 1
// @Results ethos_validation_operational == 1
void ethos_validation_verification_card(const std::map<std::string, std::string>& facts) {
    std::string command = "python3 scripts/quanta_ethos.py 'standard action'";

    std::array<char, 128> buffer;
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cout << "ethos_validation_operational = 0" << std::endl;
        return;
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    int exit_status = pclose(pipe);

    bool operational = (exit_status == 0 && result.find("decision") != std::string::npos && result.find("final_score") != std::string::npos);

    std::cout << "ethos_validation_operational = " << (operational ? 1 : 0) << std::endl;
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    ethos_validation_verification_card(facts);
    return 0;
}
