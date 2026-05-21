#include "try_alternatives.h"
#include "environment.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream> // For debug output, will remove later

// Helper for single line replacement
static void replaceSingleLine(const std::string& file,
                              int lineNumber,
                              const std::string& newText) {
    std::ifstream in(file);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (lineNumber >= 0 && lineNumber < (int)lines.size()) {
        lines[lineNumber] = newText;
    }

    std::ofstream out(file);
    if (!lines.empty()) {
        out << lines[0];
        for (size_t i = 1; i < lines.size(); ++i) {
            out << "\n" << lines[i];
        }
    }
    out.close();
}

// Helper for range replacement
static void replaceLineRange(const std::string& file,
                             int startLine,
                             int endLine,
                             const std::string& newText) {
    std::ifstream in(file);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (startLine >= 0 && startLine < (int)lines.size() &&
        endLine >= startLine && endLine < (int)lines.size()) {
        lines.erase(lines.begin() + startLine, lines.begin() + endLine + 1);
        lines.insert(lines.begin() + startLine, newText);
    } else if (startLine == 0 && lines.empty()) { // Special case for empty file and inserting at line 0
        lines.push_back(newText);
    }

    std::ofstream out(file);
    if (!lines.empty()) {
        out << lines[0];
        for (size_t i = 1; i < lines.size(); ++i) {
            out << "\n" << lines[i];
        }
    }
    out.close();
}

// Helper for inserting a line
static void insertLine(const std::string& file,
                       int lineNumber,
                       const std::string& textToInsert) {
    std::ifstream in(file);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (lineNumber >= 0 && lineNumber <= (int)lines.size()) { // <= size() for inserting at end
        lines.insert(lines.begin() + lineNumber, textToInsert);
    } else if (lineNumber == 0 && lines.empty()) { // Special case for empty file
        lines.push_back(textToInsert);
    }
    
    std::ofstream out(file);
    if (!lines.empty()) {
        out << lines[0];
        for (size_t i = 1; i < lines.size(); ++i) {
            out << "\n" << lines[i];
        }
    }
    out.close();
}

// Helper for appending to a line
static void appendToLine(const std::string& file,
                         int lineNumber,
                         const std::string& textToAppend) {
    std::ifstream in(file);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (lineNumber >= 0 && lineNumber < (int)lines.size()) {
        lines[lineNumber] += textToAppend;
    }

    std::ofstream out(file);
    if (!lines.empty()) {
        out << lines[0];
        for (size_t i = 1; i < lines.size(); ++i) {
            out << "\n" << lines[i];
        }
    }
    out.close();
}


// Helper for substring replacement in a line
static void replaceSubstringInLine(const std::string& file,
                                   int lineNumber,
                                   const std::string& findText,
                                   const std::string& replaceText) {
    std::ifstream in(file);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (lineNumber >= 0 && lineNumber < (int)lines.size()) {
        std::string& targetLine = lines[lineNumber];
        size_t pos = 0;
        while ((pos = targetLine.find(findText, pos)) != std::string::npos) {
            targetLine.replace(pos, findText.length(), replaceText);
            pos += replaceText.length();
        }
    }

    std::ofstream out(file);
    if (!lines.empty()) {
        out << lines[0];
        for (size_t i = 1; i < lines.size(); ++i) {
            out << "\n" << lines[i];
        }
    }
    out.close();
}

// Helper for global substring replacement in a file
static void replaceSubstringInFile(const std::string& file,
                                   const std::string& findText,
                                   const std::string& replaceText) {
    if (findText.empty()) return;
    
    std::ifstream in(file);
    if (!in.is_open()) return;
    
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    size_t pos = 0;
    while ((pos = content.find(findText, pos)) != std::string::npos) {
        content.replace(pos, findText.length(), replaceText);
        pos += replaceText.length();
    }

    std::ofstream out(file);
    out << content;
    out.close();
}

#include "create_reports.h"

void TryAlternatives::execute(const std::string& tempBaseDir, const std::string& projectSourceDir, const std::vector<Alternative>& alternatives) {
    for (const auto& alt : alternatives) {
        Environment env(tempBaseDir);
        env.copySource(projectSourceDir);

        std::string currentTargetFile;
        if (alt.filename.empty()) {
            currentTargetFile = env.workingDir + "/main.cpp";
        } else {
            currentTargetFile = env.workingDir + "/" + alt.filename;
        }

        std::string behavior = "";
        auto it = alternativeTypes.find(alt.type);
        if (it != alternativeTypes.end()) {
            behavior = it->second.behavior;
        }

        bool applied = true;
        if (behavior == "substring_replace_global") {
            replaceSubstringInFile(currentTargetFile, alt.findText, alt.replaceText);
        } else if (!alt.findText.empty()) {
            replaceSubstringInLine(currentTargetFile, alt.startLine, alt.findText, alt.replaceText);
        } else if (behavior == "append" || alt.type == "ADD_SEMICOLON") {
            appendToLine(currentTargetFile, alt.startLine, alt.newText);
        } else if (behavior == "insert_at_top" || alt.type == "ADD_INCLUDE" || alt.type == "DISABLE_WARNING") {
            insertLine(currentTargetFile, 0, alt.newText);
        } else if (behavior == "replace_range" || alt.type == "REPLACE_RANGE") {
            replaceLineRange(currentTargetFile, alt.startLine, alt.endLine, alt.newText);
        } else if (behavior == "replace_line" || !alt.newText.empty()) {
            replaceSingleLine(currentTargetFile, alt.startLine, alt.newText);
        } else {
            applied = false;
        }

        if (applied) {
            int status = env.runCommand("make");
            bool success = (status == 0);
            CreateReports::appendReport("data/test_runs.csv", alt, success);
        }
    }
}
