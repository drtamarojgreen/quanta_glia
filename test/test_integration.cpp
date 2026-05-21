#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

#include "log_reader.h"
#include "analyze_lines.h"
#include "produce_resolutions.h"
#include "try_alternatives.h"
#include "test_helpers.h" // Include the new helper header

namespace fs = std::filesystem;

// Mimics main.cpp's logic for testing the full pipeline
void runVisionLintPipeline(const std::vector<std::string>& inputLines, 
                           const std::string& testSrcPath, 
                           const std::string& mainContent, 
                           const std::string& makefileContent) {
    
    setupTestSrcDir(testSrcPath, mainContent, makefileContent);

    AnalyzeLines analyzer;
    auto result = analyzer.analyze(inputLines);

    auto resolutions = Config::loadResolutions("test/test-resolutions.yaml");
    auto patterns = Config::loadExtractionPatterns("test/test-extraction_patterns.yaml");
    auto altTypes = Config::loadAlternativeTypes("test/test-alternative_types.yaml");

    ProduceResolutions resolver(resolutions, patterns);
    auto alternatives = resolver.produce(result);

    TryAlternatives runner(altTypes);
    runner.execute(getTestRunBaseDir(), testSrcPath, alternatives);
    
    // Clean up the test's source directory after pipeline execution
    cleanupTestSrcDir(testSrcPath);
}

void test_full_pipeline_simple_case() {
    std::vector<std::string> inputLines = {
        "main.cpp:0: error: 'x' has incomplete type"
    };
    const std::string testSrcPath = "data/src/pipeline_simple_src";
    const std::string initialMainContent = "int main() { return 0; }\n";
    const std::string makefileContent = "all:\n\t@echo Building\n";

    runVisionLintPipeline(inputLines, testSrcPath, initialMainContent, makefileContent);

    // Verify a run directory was created
    fs::path runDir = findLatestRunDirectory();
    assert(!runDir.empty());
    assert(fs::exists(runDir));

    // Verify modification in the run directory
    // Identifier for "error: 'x' has incomplete type" will be "x".
    // For TYPE, the last resolution in the multimap (FLOAT_CAST) is applied.
    // Template for FLOAT_CAST is "static_cast<float>({identifier})"
    std::ifstream inFinal(runDir / "main.cpp");
    std::string finalContent;
    std::getline(inFinal, finalContent);
    assert(finalContent == "static_cast<float>(x)");
    inFinal.close();
    
    fs::remove_all(runDir);
}

// Optional: Add test_full_pipeline_with_stdin_input() if needed, but file input is more relevant for build systems.