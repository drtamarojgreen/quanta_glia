#ifndef GLIA_ANNOTATE_ANNOTATOR_H
#define GLIA_ANNOTATE_ANNOTATOR_H
#include <string>
#include <vector>
#include <set>

namespace glia::annotate {
struct Annotation {
    std::string repoName;
    std::set<std::string> tags;
    std::vector<std::string> headings;
};

class Annotator {
public:
    static Annotation annotate(const std::string& repoPath);
};
}
#endif
