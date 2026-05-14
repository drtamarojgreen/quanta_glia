#include <fstream>
#include <iostream>
#include <string>
#include <filesystem> // For path operations and exists check

namespace fs = std::filesystem;

int main() {
    std::string filename = "sorrel_card_test_file.tmp"; // Use a distinct name for card's temp file
    // Get the current executable's path, or current working directory.
    // For a card, it should operate in its own context. Let's assume current working directory for now.
    fs::path current_context_path = fs::current_path();
    fs::path full_path = current_context_path / filename;

    std::ofstream outfile(full_path);

    if (outfile.is_open()) {
        outfile.close();
        // Clean up immediately after observation is made, as this is a test.
        fs::remove(full_path); // Remove the file after successful creation/observation
        std::cout << "created_file_count = 1" << std::endl;
        std::cout << "filesystem_errno = 0" << std::endl;
        std::cout << "created_file_path_length = " << full_path.string().size() << std::endl;
    } else {
        std::cerr << "created_file_count = 0" << std::endl;
        std::cerr << "filesystem_errno = 1" << std::endl;
        std::cerr << "created_file_path_length = 0" << std::endl;
    }

    return 0;
}
