#ifndef GLIA_UTIL_FS_UTILS_H
#define GLIA_UTIL_FS_UTILS_H
#include <filesystem>
#include <string>
#include <vector>

namespace glia::util {
    namespace fs = std::filesystem;

    inline std::string findRepoRoot() {
        fs::path curr = fs::current_path();
        while (curr.has_parent_path()) {
            if (fs::exists(curr / "data" / "rules.xml") || fs::exists(curr / ".git")) {
                return curr.string();
            }
            curr = curr.parent_path();
        }
        return fs::current_path().string();
    }

    inline std::string findRulesXml() {
        fs::path root = findRepoRoot();
        if (fs::exists(root / "data" / "rules.xml")) {
            return (root / "data" / "rules.xml").string();
        }
        return "data/rules.xml";
    }
}
#endif
