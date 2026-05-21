#pragma once
#include <string>

struct Alternative {
    std::string filename;
    int startLine;
    int endLine;
    std::string type;
    std::string name;      // Descriptive name from resolution
    int confidence;        // Confidence score from resolution
    std::string newText;
    std::string findText;    // For substring replacement
    std::string replaceText; // For substring replacement
};
