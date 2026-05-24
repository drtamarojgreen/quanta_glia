#include "WorkflowClass.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "app/glia_workspace_cmds.h"
#include "app/glia_git_cmds.h"

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
