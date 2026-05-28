#ifndef GLIA_UTIL_STRING_UTILS_H
#define GLIA_UTIL_STRING_UTILS_H
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <cctype>

namespace glia::util {

    struct Violation {
        int line;
        std::string snippet;
    };
    inline std::string trim(const std::string& s) {
        if (s.empty()) return s;
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) start++;
        if (start == s.end()) return "";
        auto end = s.end();
        do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
        return std::string(start, end + 1);
    }

    inline std::vector<Violation> findViolations(const std::string& name, const std::string& content, const std::regex& regex) {
        std::vector<Violation> violations;
        if (name.find("Catch") != std::string::npos) {
            static const std::regex commentRegex("\\/\\/[^\\n]*|\\/\\*.*?\\*\\/", std::regex::optimize);
            auto it = std::sregex_iterator(content.begin(), content.end(), regex);
            auto end = std::sregex_iterator();
            for (; it != end; ++it) {
                std::smatch match = *it;
                std::string body = match[1].str();
                std::string stripped = std::regex_replace(body, commentRegex, "");
                stripped.erase(std::remove_if(stripped.begin(), stripped.end(), [](unsigned char ch){ return std::isspace(ch); }), stripped.end());
                if (stripped.empty()) {
                    int lineNum = std::count(content.begin(), content.begin() + match.position(), '\n') + 1;
                    violations.push_back({lineNum, match.str().substr(0, 40)});
                }
            }
        } else {
            auto it = std::sregex_iterator(content.begin(), content.end(), regex);
            auto end = std::sregex_iterator();
            for (; it != end; ++it) {
                std::smatch match = *it;
                int lineNum = std::count(content.begin(), content.begin() + match.position(), '\n') + 1;
                // Find start and end of line for snippet
                size_t start = content.find_last_of("\n", match.position());
                start = (start == std::string::npos) ? 0 : start + 1;
                size_t next_line = content.find("\n", match.position());
                size_t len = (next_line == std::string::npos) ? content.length() - start : next_line - start;
                violations.push_back({lineNum, trim(content.substr(start, len))});
            }
        }
        return violations;
    }
}
#endif
