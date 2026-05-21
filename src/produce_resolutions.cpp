#include "produce_resolutions.h"
#include <sstream>
#include <algorithm>
#include <regex> // Added for std::regex

static std::vector<std::string> tokenize(const std::string& line) {
    std::stringstream ss(line);
    std::string word;
    std::vector<std::string> tokens;
    while (ss >> word)
        tokens.push_back(word);
    return tokens;
}

// Improved identifier extraction from YAML patterns
std::string ProduceResolutions::extractIdentifier(const std::string& line) {
    for (const auto& ep : extractionPatterns) {
        if (line.find(ep.marker) != std::string::npos) {
            try {
                std::regex reg(ep.pattern);
                std::smatch match;
                if (std::regex_search(line, match, reg)) {
                    if (match.size() > 1) {
                        return match[1].str();
                    }
                }
            } catch (...) {
                // Ignore invalid regex in config
            }
        }
    }

    // Fall back to first alphanumeric token if no pattern matches
    auto tokens = tokenize(line);
    for (const auto& token : tokens) {
        if (!token.empty() && std::isalpha(token[0])) {
            std::string ident = token;
            while (!ident.empty() && std::ispunct(ident.back())) {
                ident.pop_back();
            }
            return ident;
        }
    }
    return "unknownVar";
}

// Simple heuristic rename suggestion
std::string ProduceResolutions::similarName(const std::string& name) {
    if (name.size() > 3)
        return name.substr(0, name.size() - 1);  // trim last char
    return name + "_fixed";
}

// Context-aware debug statement
std::string ProduceResolutions::debugStatement(const std::string& identifier) {
    return "std::cerr << \"DEBUG: " + identifier + " = \" << "
           + identifier + " << std::endl;";
}

// Enhanced template replacement
static std::string formatTemplate(std::string templ, 
                                  const std::string& identifier,
                                  const std::string& filename,
                                  int lineNumber) {
    auto replaceTag = [&](const std::string& tag, const std::string& val) {
        size_t pos = 0;
        while ((pos = templ.find(tag, pos)) != std::string::npos) {
            templ.replace(pos, tag.length(), val);
            pos += val.length();
        }
    };
    replaceTag("{identifier}", identifier);
    replaceTag("{filename}", filename);
    replaceTag("{line}", std::to_string(lineNumber));
    return templ;
}

std::vector<Alternative> ProduceResolutions::produce(const ErrorObject& obj) {

    std::vector<Alternative> alternatives;

    for (const auto& entry : obj.entries) {

        std::string identifier = extractIdentifier(entry.text);
        std::string suggestedName = similarName(identifier);

        // --------- Alternative 1: Rename fix ----------
        Alternative renameAlt;
        renameAlt.filename = entry.filename;
        renameAlt.startLine = entry.lineNumber;
        renameAlt.endLine = entry.lineNumber;
        renameAlt.type = "RENAME_SIMILAR";
        renameAlt.name = "Rename to similar";
        renameAlt.confidence = 7;
        renameAlt.newText = suggestedName;
        alternatives.push_back(renameAlt);

        // --------- Alternative 2: Insert Debug ----------
        Alternative debugAlt;
        debugAlt.filename = entry.filename;
        debugAlt.startLine = entry.lineNumber;
        debugAlt.endLine = entry.lineNumber;
        debugAlt.type = "INSERT_DEBUG";
        debugAlt.name = "Insert debug statement";
        debugAlt.confidence = 5;
        debugAlt.newText = debugStatement(identifier);
        alternatives.push_back(debugAlt);

        // --------- Type-specific expansion from YAML ----------
        auto range = resolutions.equal_range(entry.type);
        for (auto it = range.first; it != range.second; ++it) {
            Alternative typeAlt;
            typeAlt.filename = entry.filename;
            typeAlt.startLine = it->second.atLineZero ? 0 : entry.lineNumber;
            typeAlt.endLine = it->second.atLineZero ? 0 : entry.lineNumber;
            typeAlt.type = it->second.name;
            typeAlt.name = it->second.name;
            typeAlt.confidence = it->second.confidence;
            
            // Format template and also find/replace strings
            typeAlt.newText = formatTemplate(it->second.templateText, identifier, entry.filename, entry.lineNumber);
            typeAlt.findText = formatTemplate(it->second.findText, identifier, entry.filename, entry.lineNumber);
            typeAlt.replaceText = formatTemplate(it->second.replaceText, identifier, entry.filename, entry.lineNumber);
            
            alternatives.push_back(typeAlt);
        }
    }

    return alternatives;
}