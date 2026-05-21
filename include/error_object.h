#pragma once
#include <string>
#include <vector>

struct ErrorEntry {
    std::string type;
    std::string filename; // Added filename field
    int lineNumber;
    std::string text;
};

class ErrorObject {
public:
    ErrorObject();
    ~ErrorObject();

    int warnings;
    int typeErrors;
    int valueErrors;
    int syntaxErrors;
    int importErrors;
    int otherErrors;
    int totalErrors;

    std::vector<ErrorEntry> entries;
};
