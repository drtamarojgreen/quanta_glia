#include "RobustnessClass.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "app/glia_waste_cmds.h"

namespace fs = std::filesystem;

void robustness_verification() {
    // Create tricky file 1
    fs::path testFile1 = "robust_test_1.cpp";
    std::ofstream f1(testFile1);
    f1 << "void tricky() {\n";
    f1 << "    try { int x = 1; } catch(...) {\n";
    f1 << "        // Tricky comment that might hide empty block\n";
    f1 << "    }\n";
    f1 << "    assert(!!true); // Advanced meaningless assertion\n";
    f1 << "    int* p = new int(42); // Raw pointer\n";
    f1 << "    std::string s = \"placeholder\"; // Magic string\n";
    f1 << "}\n";
    f1.close();

    // Create tricky file 2 (for repetition)
    fs::path testFile2 = "robust_test_2.cpp";
    std::ofstream f2(testFile2);
    f2 << "void tricky_duplicate() {\n";
    f2 << "    try { int x = 1; } catch(...) {\n";
    f2 << "        // Tricky comment that might hide empty block\n";
    f2 << "    }\n";
    f2 << "    assert(!!true);\n";
    f2 << "    return { }; // placeholder return\n";
    f2 << "}\n";
    f2.close();

    glia::app::WasteScanCommand scanCmd;
    auto res1 = scanCmd.execute({"waste-scan"});
    std::cout << "robust_waste_scan_code = " << static_cast<int>(res1.code) << std::endl;

    glia::app::VerifyStructureCommand verifyCmd;
    auto res2 = verifyCmd.execute({"verify-structure"});
    std::cout << "robust_verify_structure_code = " << static_cast<int>(res2.code) << std::endl;

    glia::app::AuditRepetitionCommand repeatCmd;
    auto res3 = repeatCmd.execute({"audit-repetition"});
    std::cout << "robust_audit_repetition_code = " << static_cast<int>(res3.code) << std::endl;

    // Cleanup
    fs::remove(testFile1);
    fs::remove(testFile2);
}
