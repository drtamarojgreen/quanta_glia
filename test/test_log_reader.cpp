#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "log_reader.h"

void test_log_reader_file() {
    const std::string filename = "test_log_reader_file.txt";
    {
        std::ofstream out(filename);
        out << "line1\n";
        out << "line2\n";
        out << "line3\n";
    }

    LogReader reader;
    auto lines = reader.readFromFile(filename);

    assert(lines.size() == 3);
    assert(lines[0] == "line1");
    assert(lines[1] == "line2");
    assert(lines[2] == "line3");
}

void test_log_reader_stdin() {
    // redirect std::cin to an istringstream
    std::string input = "stdin line1\nstdin line2\n";
    std::istringstream iss(input);
    auto* oldBuf = std::cin.rdbuf(iss.rdbuf());

    LogReader reader;
    auto lines = reader.readFromStdin();

    // restore original buffer
    std::cin.rdbuf(oldBuf);

    assert(lines.size() == 2);
    assert(lines[0] == "stdin line1");
    assert(lines[1] == "stdin line2");
}
