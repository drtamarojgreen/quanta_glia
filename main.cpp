#include "log_reader.h"
#include "analyze_lines.h"
#include "produce_resolutions.h"
#include "try_alternatives.h"
#include "config.h" // Include config header
#include <iostream> // For std::cerr
#include <string>   // For std::string
#include <filesystem> // For fs::create_directories

namespace fs = std::filesystem; // Alias for filesystem

int main(int argc, char* argv[]) {
    std::vector<std::string> lines;
    std::string projectSourceDir;
    
    // Load configuration
    Config appConfig = Config::load("config.yaml");
    const std::string tempBaseDir = appConfig.getString("temp_base_dir");

    // Ensure tempBaseDir exists
    fs::create_directories(tempBaseDir);

    // Argument parsing
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file_path_or_'-'for_stdin> <project_source_directory_path>\n";
        return 1; // Exit with error
    }

    // argv[1] is the log source
    if (std::string(argv[1]) == "-") {
        lines = LogReader().readFromStdin();
    } else {
        lines = LogReader().readFromFile(argv[1]);
    }

    // argv[2] is the project source directory
    projectSourceDir = argv[2];
    
    // Load maps
    auto resolutions = Config::loadResolutions("resolutions.yaml");
    auto altTypes = Config::loadAlternativeTypes("alternative_types.yaml");
    auto extractionPatterns = Config::loadExtractionPatterns("extraction_patterns.yaml");

    AnalyzeLines analyzer;
    auto result = analyzer.analyze(lines);

    ProduceResolutions resolver(resolutions, extractionPatterns);
    auto alternatives = resolver.produce(result);

    TryAlternatives runner(altTypes);
    runner.execute(tempBaseDir, projectSourceDir, alternatives); // Pass tempBaseDir

    return 0;
}