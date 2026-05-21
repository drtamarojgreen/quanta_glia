#pragma once
#include <vector>
#include <string>
#include "error_object.h"

class AnalyzeLines {
public:
    ErrorObject analyze(const std::vector<std::string>& lines);
};
