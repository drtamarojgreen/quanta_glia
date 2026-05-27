#include "WorkflowClass.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "app/glia_workspace_cmds.h"
#include "app/glia_git_cmds.h"
#include "app/status_cmd.h"

namespace fs = std::filesystem;

void workflow_verification() {
    fs::path workspacePath = fs::current_path() / "workspace";
    fs::path testRepo = workspacePath / "test_repo";
    fs::create_directories(testRepo);

    // Initialize a dummy git repo
    std::system(("git init " + testRepo.string() + " > /dev/null 2>&1").c_str());
    std::ofstream f(testRepo / "README.md"); f << "test"; f.close();
    std::system(("cd " + testRepo.string() + " && git add . && git commit -m 'Initial' > /dev/null 2>&1").c_str());

    glia::app::WorkspaceStatusCommand statusCmd;
    auto res1 = statusCmd.execute({"workspace-status"});
    std::cout << "workflow_status_code = " << static_cast<int>(res1.code) << std::endl;

    glia::app::QuickCommitCommand quickCmd;
    std::ofstream f2(testRepo / "change.txt"); f2 << "change"; f2.close();

    auto oldPath = fs::current_path();
    fs::current_path(testRepo);
    auto res2 = quickCmd.execute({"quick-commit", "Test commit"});
    fs::current_path(oldPath);

    std::cout << "workflow_quick_commit_code = " << static_cast<int>(res2.code) << std::endl;

    // Cleanup
    fs::remove_all(testRepo);
}

void tui_verification() {
    glia::app::StatusCommand statusCmd;
    std::cout << "tui_name = " << statusCmd.name() << std::endl;
    std::cout << "tui_desc_size = " << statusCmd.description().size() << std::endl;

    std::ifstream f("src/app/status_cmd.cpp");
    if (!f.is_open()) f.open("../src/app/status_cmd.cpp");
    if (!f.is_open()) f.open("../../src/app/status_cmd.cpp");
    if (!f.is_open()) {
        std::cout << "tui_logic_marker_count = -1" << std::endl;
        return;
    }
    std::string line;
    int marker_count = 0;
    while (std::getline(f, line)) {
        if (line.find("stringstream") != std::string::npos) marker_count++;
        if (line.find("targetArgs") != std::string::npos) marker_count++;
        if (line.find("tui_header") != std::string::npos) marker_count++;
    }
    std::cout << "tui_logic_marker_count = " << marker_count << std::endl;

    // Command Palette Verification
    f.clear();
    f.seekg(0);
    int palette_marker = 0;
    while (std::getline(f, line)) {
        if (line.find("fuzzyMatch") != std::string::npos) palette_marker = 1;
    }
    std::cout << "tui_command_palette_logic_marker = " << palette_marker << std::endl;

    // Status Bar Verification
    f.clear();
    f.seekg(0);
    int status_bar_marker = 0;
    while (std::getline(f, line)) {
        if (line.find("renderStatusBar") != std::string::npos) status_bar_marker = 1;
    }
    std::cout << "tui_status_bar_logic_marker = " << status_bar_marker << std::endl;

    // Modal and Notifications Verification
    f.clear();
    f.seekg(0);
    int modal_marker = 0;
    int notifications_marker = 0;
    while (std::getline(f, line)) {
        if (line.find("TuiMode") != std::string::npos) modal_marker = 1;
        if (line.find("m_notifications") != std::string::npos) notifications_marker = 1;
    }
    std::cout << "tui_modal_logic_marker = " << modal_marker << std::endl;
    std::cout << "tui_notifications_logic_marker = " << notifications_marker << std::endl;
}
