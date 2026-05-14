// @Is platform == linux
// @Results platform_linux_detected == 1
#include <iostream>

int main() {
    #ifdef __linux__
        std::cout << "platform_linux_detected = 1" << std::endl;
    #elif _WIN32
        std::cout << "platform_linux_detected = 0" << std::endl;
    #else
        std::cout << "platform_linux_detected = 0" << std::endl;
    #endif
    std::cout << "os_probe_count = 1" << std::endl;
    return 0;
}
