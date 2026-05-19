#include "feature_flags.h"
namespace glia::app {
bool FeatureFlags::isEnabled(const std::string& feature) {
    static std::map<std::string, bool> flags = {
        {"semantic_annotation", true},
        {"adaptive_learning", false}
    };
    return flags.count(feature) && flags.at(feature);
}
}
