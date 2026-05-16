#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: phase_parsing_verification
// @Is python_available == 1
// @Results phase_parsing_operational == 1
void phase_parsing_verification_card(const std::map<std::string, std::string>& facts) {
    fs::path temp_root("tests/temp");
    std::string md_file_name = "test_phases.md";
    fs::path md_file = temp_root / md_file_name;
    {
        std::ofstream f(md_file);
        f << "## Project Phases\n";
        f << "### Phase 1\n";
        f << "- Item 1\n";
        f << "- Item 2\n";
    }

    std::string python_cmd = "python3 -c \"from scripts.phase import parse_markdown_phases; phases = parse_markdown_phases('" + md_file.string() + "'); print('Phase 1' in phases and 'Item 1' in phases['Phase 1'])\"";

    FILE* pipe = popen(python_cmd.c_str(), "r");
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    int operational = (result.find("True") != std::string::npos) ? 1 : 0;
    std::cout << "phase_parsing_operational = " << operational << std::endl;

    fs::remove(md_file);
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    phase_parsing_verification_card(facts);
    return 0;
}
