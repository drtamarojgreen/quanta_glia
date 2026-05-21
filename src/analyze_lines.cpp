#include "analyze_lines.h"
#include <algorithm>
#include <cctype>
#include <regex>

static bool containsIgnoreCase(const std::string& str, const std::string& search) {
    auto it = std::search(
        str.begin(), str.end(),
        search.begin(), search.end(),
        [](char ch1, char ch2) {
            return std::tolower(ch1) == std::tolower(ch2);
        });
    return it != str.end();
}

// Heuristic to extract filename from a log line
// Looks for patterns like "file.cpp:line:message", "file.cpp(line):message", or "file.cpp(line) message"
static std::string extractFilename(const std::string& line) {
    std::regex filename_regex(R"(^([a-zA-Z0-9_/\.-]+)(?::\d+){1,2}:?|^([a-zA-Z0-9_/\.-]+)\(\d+\):?|^([a-zA-Z0-9_/\.-]+)\s)"); // More robust filename extraction
    std::smatch match;
    if (std::regex_search(line, match, filename_regex)) {
        if (match[1].matched) return match[1].str(); // For file.cpp:10:20: or file.cpp:10:
        if (match[2].matched) return match[2].str(); // For file.cpp(10):
        if (match[3].matched) return match[3].str(); // For file.cpp (space)
    }
    return ""; // Return empty if not found
}

// Heuristic to extract line number from a log line
// Looks for patterns like "file.cpp:line:message" or "file.cpp(line):message"
static int extractLineNumber(const std::string& line) {
    std::regex line_regex(R"(:(?:(\d+)):(?:(?:\d+):)?|\((\d+)\):?)"); // Matches :10: or (10): or (10)
    std::smatch match;
    if (std::regex_search(line, match, line_regex)) {
        if (match[1].matched) return std::stoi(match[1].str());
        if (match[2].matched) return std::stoi(match[2].str());
    }
    return 0; // Default to 0 if not found
}

ErrorObject AnalyzeLines::analyze(const std::vector<std::string>& lines) {
    ErrorObject obj;

    for (const auto& line : lines) {
        ErrorEntry entry;
        entry.text = line;
        entry.lineNumber = extractLineNumber(line); // Populate line number
        entry.filename = extractFilename(line);     // Populate filename

        if (containsIgnoreCase(line, "warning")) {
            entry.type = "WARNING";
            obj.warnings++;
            obj.entries.push_back(entry);
        }

        if (containsIgnoreCase(line, "error")) {
            if (containsIgnoreCase(line, "type")) {
                entry.type = "TYPE";
                obj.typeErrors++;
            }
            else if (containsIgnoreCase(line, "value")) {
                entry.type = "VALUE";
                obj.valueErrors++;
            }
            else if (containsIgnoreCase(line, "syntax")) {
                entry.type = "SYNTAX";
                obj.syntaxErrors++;
            }
            else if (containsIgnoreCase(line, "import")) {
                entry.type = "IMPORT";
                obj.importErrors++;
            }
            else {
                entry.type = "ERROR";
                obj.otherErrors++;
            }

            obj.entries.push_back(entry);
        }
    }

    obj.totalErrors = obj.typeErrors +
                      obj.valueErrors +
                      obj.syntaxErrors +
                      obj.importErrors +
                      obj.otherErrors;

    return obj;
}
