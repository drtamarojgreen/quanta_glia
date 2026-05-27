#include <iostream>
#include <cassert>
#include <filesystem>
#include "app/glia_qtl_cmd.h"
#include "util/fs_utils.h"

namespace fs = std::filesystem;

void test_qtl_loading() {
    std::cout << "Running test_qtl_loading..." << std::endl;
    glia::app::QtlCommand cmd;
    // Private method access not possible without hacks or making it public
    // but we can test the public execute method
    auto res = cmd.execute({"qtl"});
    assert(res.code == glia::core::ExitCode::Success);
    std::cout << "test_qtl_loading passed." << std::endl;
}

void test_qtl_file_output() {
    std::cout << "Running test_qtl_file_output..." << std::endl;
    glia::app::QtlCommand cmd;
    std::string testFile = "test_qtl_output.txt";
    auto res = cmd.execute({"qtl", "--filename", testFile});
    assert(res.code == glia::core::ExitCode::Success);
    assert(fs::exists(testFile));
    fs::remove(testFile);
    std::cout << "test_qtl_file_output passed." << std::endl;
}

int main() {
    test_qtl_loading();
    test_qtl_file_output();
    return 0;
}
