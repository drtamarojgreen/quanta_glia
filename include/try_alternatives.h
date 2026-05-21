#pragma once
#include <vector>
#include "alternative.h"

#include <map>
#include "config.h"

class TryAlternatives {
public:
    TryAlternatives() = default;
    TryAlternatives(const std::map<std::string, AlternativeType>& altTypes) : alternativeTypes(altTypes) {}

    void execute(const std::string& tempBaseDir, const std::string& projectSourceDir, const std::vector<Alternative>& alternatives);

private:
    std::map<std::string, AlternativeType> alternativeTypes;
};
