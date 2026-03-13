#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

// @Card: quanta_glia_extraction
// @Is python_available == true
// @Results quanta_glia_extraction_operational == true
void quanta_glia_extraction_card() {
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
}

// @Card: max_repos_limit_verification
// @Is python_available == true
// @Results quanta_glia_max_repos_limit_operational == true
void max_repos_limit_verification_card() {
    int max_repos = 10; // Default in config.yaml
    int num_to_create = max_repos + 1;
    std::string repo_prefix = "chai_max_test_";
    std::string repo_list = "";

    for (int i = 1; i <= num_to_create; ++i) {
        std::string repo_name = repo_prefix + std::to_string(i);
        fs::create_directory(repo_name);
        std::ofstream readme(fs::path(repo_name) / "README.md");
        readme << "test";
        readme.close();
        repo_list += " " + repo_name;
    }

    std::string command = "python3 scripts/quanta_glia.py " + repo_list + " > /dev/null 2>&1";
    std::system(command.c_str());

    int count = 0;
    if (fs::exists("knowledge_base")) {
        for (auto const& dir_entry : fs::directory_iterator("knowledge_base")) {
            if (dir_entry.is_directory() && dir_entry.path().filename().string().find(repo_prefix) == 0) {
                count++;
            }
        }
    }

    std::cout << "quanta_glia_max_repos_limit_operational = " << (count == max_repos ? "true" : "false") << std::endl;

    // Cleanup
    for (int i = 1; i <= num_to_create; ++i) {
        std::string repo_name = repo_prefix + std::to_string(i);
        fs::remove_all(repo_name);
        if (fs::exists("knowledge_base/" + repo_name)) {
            fs::remove_all("knowledge_base/" + repo_name);
        }
    }
}

// @Card: custom_config_verification
// @Is python_available == true
// @Results quanta_glia_custom_config_operational == true
void custom_config_verification_card() {
    std::string repo_name = "chai_custom_config_repo";
    std::string config_file = "tests/bdd/custom_config.yaml";
    fs::path repo_path(repo_name);

    fs::create_directory(repo_path);
    {
        std::ofstream f(repo_path / "README.md");
        f << "should be skipped";
    }
    {
        std::ofstream f(repo_path / "IMPORTANT_FILE.txt");
        f << "should be extracted";
    }

    std::string command = "python3 scripts/quanta_glia.py --config " + config_file + " " + repo_name + " > /dev/null 2>&1";
    std::system(command.c_str());

    fs::path kb_path = fs::path("knowledge_base") / repo_name;
    bool important_exists = fs::exists(kb_path / "IMPORTANT_FILE.txt");
    bool readme_exists = fs::exists(kb_path / "README.md");

    std::cout << "quanta_glia_custom_config_operational = " << (important_exists && !readme_exists ? "true" : "false") << std::endl;

    // Cleanup
    fs::remove_all(repo_path);
    if (fs::exists(kb_path)) {
        fs::remove_all(kb_path);
    }
}

// @Card: no_target_files_verification
// @Is python_available == true
// @Results quanta_glia_no_target_files_operational == true
void no_target_files_verification_card() {
    std::string repo_name = "chai_no_targets_repo";
    fs::path repo_path(repo_name);

    fs::create_directory(repo_path);
    {
        std::ofstream f(repo_path / "some_other_file.txt");
        f << "This is not a target file.";
    }

    std::string command = "python3 scripts/quanta_glia.py " + repo_name + " > /dev/null 2>&1";
    std::system(command.c_str());

    fs::path kb_path = fs::path("knowledge_base") / repo_name;
    bool kb_exists = fs::exists(kb_path);

    std::cout << "quanta_glia_no_target_files_operational = " << (!kb_exists ? "true" : "false") << std::endl;

    // Cleanup
    fs::remove_all(repo_path);
    if (kb_exists) {
        fs::remove_all(kb_path);
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "max_repos") {
            max_repos_limit_verification_card();
        } else if (arg == "custom_config") {
            custom_config_verification_card();
        } else if (arg == "no_targets") {
            no_target_files_verification_card();
        } else {
            quanta_glia_extraction_card();
        }
    } else {
        quanta_glia_extraction_card();
    }
    return 0;
}
