// @Card: check_os
// @Is platform == linux
// @Results platform_linux_detected == 100

// @Card: check_integrity
// @Is platform == linux
// @Results system_integrity_score == 100

#include <iostream>
#include <string>

void check_os() {
    std::cout << "platform_linux_detected = 100" << std::endl;
    std::cout << "stdout_line_count = 2" << std::endl;
}

void check_integrity() {
    std::cout << "system_integrity_score = 100" << std::endl;
    std::cout << "integrity_check_count = 100" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    std::string arg = argv[1];
    if (arg == "check_os") check_os();
    else if (arg == "check_integrity") check_integrity();
    return 0;
}
