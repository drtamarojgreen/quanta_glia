#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm> // For std::remove_if
#include <cctype>    // For std::isspace

// Helper to trim leading/trailing whitespace
static std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v"; // Correct string literal
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return str; // Or return empty string if you prefer empty result for all-whitespace
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}
Config Config::load(const std::string& configFilePath) {
    Config config;
    std::ifstream file(configFilePath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configFilePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Ignore empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            // Malformed line, ignore or throw error depending on strictness
            // For now, let's ignore
            continue;
        }

        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));

        if (!key.empty()) {
            config.values[key] = value;
        }
    }

    return config;
}

static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

static void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::multimap<std::string, Resolution> Config::loadResolutions(const std::string& resolutionsFilePath) {
    std::multimap<std::string, Resolution> resolutions;
    std::ifstream file(resolutionsFilePath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open resolutions file: " + resolutionsFilePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string errorCode = trim(line.substr(0, colonPos));
        std::string rest = line.substr(colonPos + 1);

        std::vector<std::string> parts = split(rest, '|');
        if (parts.size() < 2) continue;

        Resolution res;
        res.errorCode = errorCode;
        res.name = parts[0];
        res.templateText = parts[1];
        res.confidence = (parts.size() > 2) ? std::stoi(parts[2]) : 5;
        res.atLineZero = (parts.size() > 3) ? (parts[3] == "true" || parts[3] == "1") : false;
        res.findText = (parts.size() > 4) ? parts[4] : "";
        res.replaceText = (parts.size() > 5) ? parts[5] : "";

        // Support escaped newlines
        replaceAll(res.templateText, "\\n", "\n");
        replaceAll(res.findText, "\\n", "\n");
        replaceAll(res.replaceText, "\\n", "\n");

        if (!errorCode.empty()) {
            resolutions.insert({errorCode, res});
        }
    }
    return resolutions;
}

std::map<std::string, AlternativeType> Config::loadAlternativeTypes(const std::string& alternativeTypesFilePath) {
    std::map<std::string, AlternativeType> alternativeTypes;
    std::ifstream file(alternativeTypesFilePath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open alternative types file: " + alternativeTypesFilePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string type = trim(line.substr(0, colonPos));
        std::string behavior = trim(line.substr(colonPos + 1));

        if (!type.empty()) {
            AlternativeType altType;
            altType.type = type;
            altType.behavior = behavior;
            alternativeTypes[type] = altType;
        }
    }
    return alternativeTypes;
}

std::vector<ExtractionPattern> Config::loadExtractionPatterns(const std::string& filePath) {
    std::vector<ExtractionPattern> patterns;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open extraction patterns: " + filePath);
    }
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        patterns.push_back({trim(line.substr(0, colonPos)), trim(line.substr(colonPos + 1))});
    }
    return patterns;
}

std::string Config::getString(const std::string& key) const {
    auto it = values.find(key);
    if (it == values.end()) {
        throw std::runtime_error("Config key not found: " + key);
    }
    return it->second;
}