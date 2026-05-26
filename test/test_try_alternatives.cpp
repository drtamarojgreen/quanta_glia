#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "try_alternatives.h"
#include "alternative.h"
#include "test_helpers.h" // Include the  "new" helper header

namespace fs = std::filesystem;

// This test focuses on the high-level behavior without relying on a real ./src tree.
// We create a minimal ./src with a main.cpp and a dummy Makefile so that execute()
// can run without throwing from filesystem or make.
void test_try_alternatives_basic_execute() {
    const std::string testSrcPath = "src/basic_execute_src";
    const std::string mainContent = "int main(){return 0;}\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    setupTestSrcDir(testSrcPath, mainContent, makefileContent);

    // create one COMMENT_LINE alternative
    Alternative alt;
    alt.filename = "main.cpp"; // Now populate filename
    alt.startLine = 0;
    alt.endLine = 0;
    alt.type = "REPLACE_LINE";
    alt.newText = "// replaced line";

    std::vector<Alternative> alts = {alt};

    TryAlternatives runner;
    runner.execute(getTestRunBaseDir(), testSrcPath, alts); // Pass tempBaseDir and projectSourceDir

    // Assert that the original testSrcPath/main.cpp is unchanged by the runner's execution
    // (runner operates on a copy)
    std::ifstream in(testSrcPath + "/main.cpp");
    std::string content;
    std::getline(in, content);
    assert(content == mainContent.substr(0, mainContent.length() - 1)); // Remove trailing newline for assert

    // Find the latest run directory and verify the content there
    fs::path runDir = findLatestRunDirectory();
    assert(!runDir.empty());
    
    std::ifstream run_in(runDir / "main.cpp");
    std::string run_content;
    std::getline(run_in, run_content);
    assert(run_content == "// replaced line");

    cleanupTestSrcDir(testSrcPath); // Clean up the test's source directory
}

void test_try_alternatives_replace_line() {
    const std::string testSrcPath = "src/replace_line_src";
    const std::string mainContent = "line 1\nline 2 to be replaced\nline 3\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    setupTestSrcDir(testSrcPath, mainContent, makefileContent);

    // create a REPLACE_LINE alternative
    Alternative alt;
    alt.filename = "main.cpp";
    alt.startLine = 1; // 0-indexed, so this is the second line
    alt.endLine = 1;
    alt.type = "REPLACE_LINE";
    alt.newText = "replaced line 2";

    std::vector<Alternative> alts = {alt};

    TryAlternatives runner;
    runner.execute(getTestRunBaseDir(), testSrcPath, alts); // Pass tempBaseDir and projectSourceDir

    // Assert that the original testSrcPath/main.cpp is unchanged
    std::ifstream in_original(testSrcPath + "/main.cpp");
    std::string line_original;
    std::getline(in_original, line_original); assert(line_original == "line 1");
    std::getline(in_original, line_original); assert(line_original == "line 2 to be replaced");
    std::getline(in_original, line_original); assert(line_original == "line 3");

    // Find the latest run directory and verify the content there
    fs::path runDir = findLatestRunDirectory();
    assert(!runDir.empty());
    
    std::ifstream run_in(runDir / "main.cpp");
    std::string line;
    std::getline(run_in, line);
    assert(line == "line 1");
    std::getline(run_in, line);
    assert(line == "replaced line 2");
    std::getline(run_in, line);
    assert(line == "line 3");

    cleanupTestSrcDir(testSrcPath);
}

void test_try_alternatives_replace_range() {
    const std::string testSrcPath = "src/replace_range_src";
    const std::string mainContent = "line 1\nline 2 to be replaced\nline 3 to be replaced\nline 4\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    setupTestSrcDir(testSrcPath, mainContent, makefileContent);

    // create a REPLACE_RANGE alternative
    Alternative alt;
    alt.filename = "main.cpp";
    alt.startLine = 1; // 0-indexed, so this is the second line
    alt.endLine = 2;   // 0-indexed, so this is the third line (inclusive)
    alt.type = "REPLACE_RANGE";
    alt.newText = "replaced range content";

    std::vector<Alternative> alts = {alt};

    TryAlternatives runner;
    runner.execute(getTestRunBaseDir(), testSrcPath, alts); // Pass tempBaseDir and projectSourceDir

    // Assert that the original testSrcPath/main.cpp is unchanged
    std::ifstream in_original(testSrcPath + "/main.cpp");
    std::string line_original;
    std::getline(in_original, line_original); assert(line_original == "line 1");
    std::getline(in_original, line_original); assert(line_original == "line 2 to be replaced");
    std::getline(in_original, line_original); assert(line_original == "line 3 to be replaced");
    std::getline(in_original, line_original); assert(line_original == "line 4");

    // Find the latest run directory and verify the content there
    fs::path runDir = findLatestRunDirectory();
    assert(!runDir.empty());
    
    std::ifstream run_in(runDir / "main.cpp");
    std::string line;
    std::getline(run_in, line);
    assert(line == "line 1");
    std::getline(run_in, line);
    assert(line == "replaced range content");
    std::getline(run_in, line);
    assert(line == "line 4");
    assert(run_in.eof());

    cleanupTestSrcDir(testSrcPath);
}

void test_try_alternatives_multiple_alternatives() {
    const std::string testSrcPath = "src/multiple_alts_src";
    const std::string mainContent = "line 1 - original\nline 2 - to be commented\nline 3 - to be replaced\nline 4 - start of range\nline 5 - middle of range\nline 6 - end of range\nline 7 - original\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    setupTestSrcDir(testSrcPath, mainContent, makefileContent);

    // Clear previous runs to make counting easier
    if (fs::exists(getTestRunBaseDir())) {
        fs::remove_all(getTestRunBaseDir());
        fs::create_directories(getTestRunBaseDir());
    }

    std::vector<Alternative> alts;

    // Alternative 1: COMMENT_LINE
    Alternative alt1;
    alt1.filename = "main.cpp";
    alt1.startLine = 1; // line 2
    alt1.endLine = 1;
    alt1.type = "REPLACE_LINE"; // Use REPLACE_LINE for simplicity in trial
    alt1.newText = "// line 2 - commented";
    alts.push_back(alt1);

    // Alternative 2: REPLACE_LINE
    Alternative alt2;
    alt2.filename = "main.cpp";
    alt2.startLine = 2; // line 3
    alt2.endLine = 2;
    alt2.type = "REPLACE_LINE";
    alt2.newText = "line 3 - replaced";
    alts.push_back(alt2);

    // Alternative 3: REPLACE_RANGE
    Alternative alt3;
    alt3.filename = "main.cpp";
    alt3.startLine = 3; // line 4
    alt3.endLine = 5;   // line 6 (inclusive)
    alt3.type = "REPLACE_RANGE";
    alt3.newText = "lines 4-6 - replaced range";
    alts.push_back(alt3);

    // Define behaviors
    std::map<std::string, AlternativeType> altTypes;
    altTypes["REPLACE_LINE"] = {"REPLACE_LINE", "replace_line"};
    altTypes["REPLACE_RANGE"] = {"REPLACE_RANGE", "replace_range"};

    TryAlternatives runner(altTypes);
    runner.execute(getTestRunBaseDir(), testSrcPath, alts);

    // Verify that 3 run directories were created
    int runCount = 0;
    for (const auto& entry : fs::directory_iterator(getTestRunBaseDir())) {
        if (entry.is_directory() && entry.path().filename().string().rfind("run_", 0) == 0) {
            runCount++;
        }
    }
    assert(runCount == 3);

    // Find the latest run directory (should correspond to alt3)
    fs::path runDir = findLatestRunDirectory();
    assert(!runDir.empty());
    
    std::ifstream run_in(runDir / "main.cpp");
    std::string line;

    std::getline(run_in, line);
    assert(line == "line 1 - original");
    std::getline(run_in, line);
    assert(line == "line 2 - to be commented"); // Unchanged in this trial!
    std::getline(run_in, line);
    assert(line == "line 3 - to be replaced");  // Unchanged in this trial!
    std::getline(run_in, line);
    assert(line == "lines 4-6 - replaced range"); // Only this is changed
    std::getline(run_in, line);
    assert(line == "line 7 - original");
    assert(run_in.eof());

    // Verify CSV report exists
    assert(fs::exists("test_runs.csv"));

    cleanupTestSrcDir(testSrcPath);
}

void test_try_alternatives_multiple_runs() {
    const std::string testSrcPath = "src/multiple_runs_src";
    const std::string mainContent = "initial line\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    setupTestSrcDir(testSrcPath, mainContent, makefileContent);

    // Capture initial content of main.cpp
    std::string initial_main_content;
    {
        std::ifstream in(testSrcPath + "/main.cpp");
        std::string line;
        std::getline(in, line);
        initial_main_content = line;
    }

    // First run
    Alternative alt1;
    alt1.filename = "main.cpp";
    alt1.startLine = 0;
    alt1.endLine = 0;
    alt1.type = "REPLACE_LINE";
    alt1.newText = "first run content";
    std::vector<Alternative> alts1 = {alt1};

    TryAlternatives runner;
    runner.execute(getTestRunBaseDir(), testSrcPath, alts1); // Pass tempBaseDir and projectSourceDir
    fs::path runDir1 = findLatestRunDirectory();
    assert(!runDir1.empty());

    // Second run
    Alternative alt2;
    alt2.filename = "main.cpp";
    alt2.startLine = 0;
    alt2.endLine = 0;
    alt2.type = "REPLACE_LINE";
    alt2.newText = "second run content";
    std::vector<Alternative> alts2 = {alt2};

    runner.execute(getTestRunBaseDir(), testSrcPath, alts2); // Pass tempBaseDir and projectSourceDir
    fs::path runDir2 = findLatestRunDirectory();
    assert(!runDir2.empty());

    // Assert that original testSrcPath/main.cpp is unchanged
    std::ifstream in_original(testSrcPath + "/main.cpp");
    std::string content_original;
    std::getline(in_original, content_original);
    assert(content_original == initial_main_content);

    // Assert that two distinct run directories were created
    assert(runDir1 != runDir2);

    // Verify content of first run directory
    std::ifstream run1_in(runDir1 / "main.cpp");
    std::string run1_content;
    std::getline(run1_in, run1_content);
    assert(run1_content == "first run content");

    // Verify content of second run directory
    std::ifstream run2_in(runDir2 / "main.cpp");
    std::string run2_content;
    std::getline(run2_in, run2_content);
    assert(run2_content == "second run content");

    cleanupTestSrcDir(testSrcPath);
}