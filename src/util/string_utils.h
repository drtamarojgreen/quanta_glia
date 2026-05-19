#ifndef GLIA_UTIL_STRING_UTILS_H
#define GLIA_UTIL_STRING_UTILS_H
#include <string>
#include <vector>
#include <algorithm>

namespace glia::util {
    inline std::string trim(const std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) start++;
        auto end = s.end();
        do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
        return std::string(start, end + 1);
    }
}
#endif
