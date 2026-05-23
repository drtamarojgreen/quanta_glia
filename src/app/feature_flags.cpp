#include "feature_flags.h"
namespace glia::app {
std::map<std::string, bool> FeatureFlags::flags() {
    return {{"modular_xml", true}, {"genome_api", false}};
}
}
