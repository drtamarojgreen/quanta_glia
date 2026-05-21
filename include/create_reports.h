#pragma once
#include <string>
#include "alternative.h"

class CreateReports {
public:
    static void appendReport(const std::string& csvPath, const Alternative& alt, bool success);
};
