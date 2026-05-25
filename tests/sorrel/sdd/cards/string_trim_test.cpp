#include <iostream>
#include <string>
// Include the  "new" utility header
#include "cpp/util/string_utils.h"

int main() {
    std::string test1 = "  hello  ";
    std::string test2 = "hello";
    std::string test3 = "  hello world  ";
    std::string test4 = "";
    std::string test5 = "     ";
    // Corrected string literal for test6
    std::string test6 = "\t\nleading and trailing\r\n";

    int mismatch_count = 0;
    if (Sorrel::Cpp::Util::trim(test1) != "hello") mismatch_count++;
    if (Sorrel::Cpp::Util::trim(test2) != "hello") mismatch_count++;
    if (Sorrel::Cpp::Util::trim(test3) != "hello world") mismatch_count++;
    if (Sorrel::Cpp::Util::trim(test4) != "") mismatch_count++;
    if (Sorrel::Cpp::Util::trim(test5) != "") mismatch_count++;
    if (Sorrel::Cpp::Util::trim(test6) != "leading and trailing") mismatch_count++;

    std::cout << "string_trim_case_count = 6" << std::endl;
    std::cout << "string_trim_mismatch_count = " << mismatch_count << std::endl;

    return 0;
}
