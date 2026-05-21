#pragma once
#include <vector>
#include <string>

class LogReader {
public:
    std::vector<std::string> readFromFile(const std::string& filename);
    std::vector<std::string> readFromStdin();
};
