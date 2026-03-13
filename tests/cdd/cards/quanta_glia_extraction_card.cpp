#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

int main() {
    std::string repo_name = "chai_dummy_repo";
    fs::path repo_path(repo_name);

    // Setup: Create a dummy repository with a README
    fs::create_directory(repo_path);
    std::ofstream readme(repo_path / "README.md");
    readme << "Test Readme Content";
    readme.close();

    // Execute Quanta Glia script via system call
    std::string command = "python3 scripts/quanta_glia.py " + repo_name + " > /dev/null 2>&1";
    int result = std::system(command.c_str());

    // Verify: Check if the extracted file exists in the knowledge base
    fs::path kb_path = fs::path("knowledge_base") / repo_name / "README.md";
    bool operational = (result == 0) && fs::exists(kb_path);

    std::cout << "quanta_glia_extraction_operational = " << (operational ? "true" : "false") << std::endl;

    // Cleanup
    fs::remove_all(repo_path);
    if (fs::exists("knowledge_base/" + repo_name)) {
        fs::remove_all("knowledge_base/" + repo_name);
    }

    return 0;
}
