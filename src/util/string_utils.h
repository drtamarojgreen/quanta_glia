#ifndef GLIA_UTIL_STRING_UTILS_H
#define GLIA_UTIL_STRING_UTILS_H
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace glia::util {
    inline std::string trim(const std::string& s) {
        if (s.empty()) return s;
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) start++;
        if (start == s.end()) return "";
        auto end = s.end();
        do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
        return std::string(start, end + 1);
    }

    inline bool fuzzyMatch(const std::string& query, const std::string& target) {
        if (query.empty()) return true;
        size_t queryIdx = 0;
        size_t targetIdx = 0;
        while (queryIdx < query.length() && targetIdx < target.length()) {
            if (std::tolower(query[queryIdx]) == std::tolower(target[targetIdx])) {
                queryIdx++;
            }
            targetIdx++;
        }
        return queryIdx == query.length();
    }
}
#endif
