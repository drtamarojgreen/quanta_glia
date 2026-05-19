#ifndef GLIA_CURATE_CURATOR_H
#define GLIA_CURATE_CURATOR_H
#include <string>
#include <vector>
#include "../annotate/annotator.h"

namespace glia::curate {
struct KnowledgeBundle {
    std::string name;
    std::vector<std::string> repoNames;
};

class Curator {
public:
    static std::vector<KnowledgeBundle> suggestBundles(const std::vector<glia::annotate::Annotation>& annotations);
};
}
#endif
