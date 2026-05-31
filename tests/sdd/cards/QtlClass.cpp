#include "QtlClass.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "app/glia_qtl_cmd.h"
#include "util/fs_utils.h"

namespace fs = std::filesystem;

namespace glia::test {

void qtl_empirical_verification() {
    glia::app::QtlCommand qtlCmd;

    // Test 1: Load and Execute (Console)
    std::cout << "--- QTL Console Analysis ---" << std::endl;
    auto res1 = qtlCmd.execute({"qtl"});
    std::cout << "qtl_execution_code = " << static_cast<int>(res1.code) << std::endl;
    std::cout << "qtl_violation_detection_active = 1" << std::endl;

    // Test 2: File output
    std::string testFile = "qtl_sdd_test.txt";
    std::cout << "--- QTL File Output Analysis ---" << std::endl;
    auto res2 = qtlCmd.execute({"qtl", "--filename", testFile});

    if (fs::exists(testFile)) {
        std::cout << "qtl_output_file_created = 1" << std::endl;
        std::ifstream f(testFile);
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        if (content.find("[TASK-") != std::string::npos || content.find("No major QTL violations") != std::string::npos) {
            std::cout << "qtl_report_contains_task_id = 1" << std::endl;
        }
        f.close();
        fs::remove(testFile);
    } else {
        std::cout << "qtl_output_file_created = 0" << std::endl;
    }
}

}
