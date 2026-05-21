#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include <vector>

struct Resolution {
    std::string errorCode;
    std::string name;
    std::string templateText;
    int confidence;
    bool atLineZero;
    std::string findText;
    std::string replaceText;
};

struct AlternativeType {
    std::string type;
    std::string behavior;
};

struct ExtractionPattern {
    std::string marker;
    std::string pattern;
};

class Config {
public:
    static Config load(const std::string& configFilePath);
    static std::multimap<std::string, Resolution> loadResolutions(const std::string& resolutionsFilePath);
    static std::map<std::string, AlternativeType> loadAlternativeTypes(const std::string& alternativeTypesFilePath);
    static std::vector<ExtractionPattern> loadExtractionPatterns(const std::string& filePath);

    std::string getString(const std::string& key) const;

private:
    std::map<std::string, std::string> values;
    Config() = default;
};