#include <iostream>
#include <fstream>
#include <string>
#include <filesystem> // Required for C++17 filesystem operations
#include "cpp/util/string_utils.h" // Include the new utility header

namespace fs = std::filesystem;

// Function to find the 'cdd' directory based on the executable's path
fs::path get_executable_cdd_directory(const char* argv0) {
    fs::path executable_path = fs::canonical(argv0); // Get absolute path to executable
    fs::path executable_parent_path = executable_path.parent_path(); // Should be tests/chai/cdd/

    // Assuming card_runner is directly inside tests/chai/cdd/
    if (fs::exists(executable_parent_path) && fs::is_directory(executable_parent_path)) {
        return executable_parent_path;
    }
    return {}; // Return empty path if not found
}

int main(int argc, char* argv[]) {
    std::cout << "CHAI card runner" << std::endl;

    fs::path cdd_dir = get_executable_cdd_directory(argv[0]);
    if (cdd_dir.empty()) {
        std::cerr << "Error: 'cdd' directory not found from executable path." << std::endl;
        return 1;
    }

    fs::path facts_file_path = cdd_dir / "facts" / "environment.facts";

    std::cout << "Attempting to read facts from: " << facts_file_path << std::endl;

    std::ifstream facts_file(facts_file_path);
    if (facts_file.is_open()) {
        std::cout << "--- Parsed Facts Content ---" << std::endl;
        std::string line;
        bool has_facts = false;
        while (std::getline(facts_file, line)) {
            // Apply trim to the whole line first
            std::string trimmed_line = Chai::Cpp::Util::trim(line);

            if (trimmed_line.empty() || trimmed_line[0] == '#') { // Skip empty lines and comments
                continue; // Skip to next line
            }

            // Process only non-empty, non-comment lines
            size_t eq_pos = trimmed_line.find('=');
            if (eq_pos != std::string::npos) {
                // Apply trim to key and value from the trimmed_line
                std::string key = Chai::Cpp::Util::trim(trimmed_line.substr(0, eq_pos));
                std::string value = Chai::Cpp::Util::trim(trimmed_line.substr(eq_pos + 1));
                std::cout << "Parsed - Key: '" << key << "', Value: '" << value << "'" << std::endl;
            } else {
                std::cout << "Parsed - No '=' found in line: '" << trimmed_line << "'" << std::endl;
            }
            has_facts = true;
        }
        if (!has_facts) {
            std::cout << "(no non-empty facts recorded)" << std::endl;
        }
        std::cout << "--- End Parsed Facts Content ---" << std::endl;
        facts_file.close();
    } else {
        std::cerr << "Error: Could not open facts file: " << facts_file_path << std::endl;
        return 1;
    }

    return 0;
}
