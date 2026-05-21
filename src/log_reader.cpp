#include "log_reader.h"
#include <fstream>
#include <iostream>

std::vector<std::string> LogReader::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line))
        lines.push_back(line);

    return lines;
}

std::vector<std::string> LogReader::readFromStdin() {
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(std::cin, line))
        lines.push_back(line);

    return lines;
}
