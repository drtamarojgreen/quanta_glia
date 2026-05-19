#ifndef GLIA_APP_FEATURE_FLAGS_H
#define GLIA_APP_FEATURE_FLAGS_H
#include <string>
#include <map>

namespace glia::app {
class FeatureFlags {
public:
    static bool isEnabled(const std::string& feature);
};
}
#endif
