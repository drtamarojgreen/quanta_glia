#include <cassert>
#include <iostream>
#include <vector>
#include "cli/cli.h"
#include "util/string_utils.h"

using namespace glia::cli;
using namespace glia::util;

void test_table_utility() {
    std::vector<std::string> headers = {"A", "B"};
    std::vector<std::vector<std::string>> rows = {{"z", "2"}, {"a", "1"}};

    std::cout << "Testing table sorting (visual check needed if manual):\n";
    Terminal::printTable(headers, rows, true);
    Terminal::printTable(headers, rows, false);
}

void test_fuzzy_match() {
    assert(fuzzyMatch("abc", "Abc") == true);
    assert(fuzzyMatch("abc", "Abc", true) == false);
    assert(fuzzyMatch("/[0-9]/", "item1") == true);
    assert(fuzzyMatch("/[0-9]/", "item") == false);
    std::cout << "test_fuzzy_match passed" << std::endl;
}

int main() {
    test_table_utility();
    test_fuzzy_match();
    return 0;
}
