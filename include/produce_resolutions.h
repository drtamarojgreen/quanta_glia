#pragma once
#include <vector>
#include <map>
#include "error_object.h"
#include "alternative.h"
#include "config.h"

class ProduceResolutions {
public:
    ProduceResolutions() = default;
    ProduceResolutions(const std::multimap<std::string, Resolution>& resolutionsMap,
                       const std::vector<ExtractionPattern>& patterns) 
        : resolutions(resolutionsMap), extractionPatterns(patterns) {}

    std::vector<Alternative> produce(const ErrorObject& obj);

private:
    std::multimap<std::string, Resolution> resolutions;
    std::vector<ExtractionPattern> extractionPatterns;
    std::string extractIdentifier(const std::string& line);
    std::string similarName(const std::string& name);
    std::string debugStatement(const std::string& identifier);
};
