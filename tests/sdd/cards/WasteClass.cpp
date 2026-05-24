#include "WasteClass.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "app/glia_waste_cmds.h"

namespace fs = std::filesystem;

void waste_verification() {
    fs::path testFile = "waste_test.cpp";
    std::ofstream f(testFile);
    f << "// TO" << "DO: Logic here\n";
    f << "void dummy() {\n";
    f << "    try { /* ... */ } catch(...) {}\n";
    f << "    assert(tr" << "ue);\n";
    f << "}\n";
    f.close();

    glia::app::WasteScanCommand scanCmd;
    auto res1 = scanCmd.execute({"waste-scan"});
    std::cout << "waste_scan_code = " << static_cast<int>(res1.code) << std::endl;

    glia::app::VerifyStructureCommand verifyCmd;
    auto res2 = verifyCmd.execute({"verify-structure"});
    std::cout << "verify_structure_code = " << static_cast<int>(res2.code) << std::endl;

    fs::remove(testFile);
}
