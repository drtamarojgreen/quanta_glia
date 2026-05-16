#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: enhancement_parsing_verification
// @Is python_available == 1
// @Results enhancement_parsing_operational == 1
void enhancement_parsing_verification_card(const std::map<std::string, std::string>& facts) {
    fs::path temp_root("tests/temp");
    std::string md_file_name = "test_enhancements.md";
    fs::path md_file = temp_root / md_file_name;
    {
        std::ofstream f(md_file);
        f << "## Proposed Enhancements for Module A\n";
        f << "- [ ] Enhancement 1\n";
    }

    // Note: lstrip('- ') removes '- ' but also '[' if not careful.
    // Actually enhancement.py lstrip('- ') will leave '[ ] Enhancement 1'
    std::string python_cmd = "python3 -c \"from scripts.enhancement import parse_markdown_enhancements; enh = parse_markdown_enhancements('" + md_file.string() + "'); print('Module A' in enh and '[ ] Enhancement 1' in enh['Module A'])\"";

    FILE* pipe = popen(python_cmd.c_str(), "r");
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    int parsed_enhancements_count = 0;
    if (result.find("True") != std::string::npos) {
        parsed_enhancements_count = 1; // Simplification for this test card
    }
    std::cout << "parsed_enhancements_count = " << parsed_enhancements_count << std::endl;

    fs::remove(md_file);
}

int main() {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    enhancement_parsing_verification_card(facts);
    return 0;
}
