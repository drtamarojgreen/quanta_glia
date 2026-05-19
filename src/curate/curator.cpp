#include "curator.h"
#include <map>

namespace glia::curate {
std::vector<KnowledgeBundle> Curator::suggestBundles(const std::vector<glia::annotate::Annotation>& annotations) {
    std::map<std::string, KnowledgeBundle> bundles;
    for (const auto& ann : annotations) {
        for (const auto& tag : ann.tags) {
            bundles[tag].name = tag;
            bundles[tag].repoNames.push_back(ann.repoName);
        }
    }
    std::vector<KnowledgeBundle> result;
    for (auto const& [name, bundle] : bundles) result.push_back(bundle);
    return result;
}
}
