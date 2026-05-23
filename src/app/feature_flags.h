#ifndef GLIA_APP_FEATURE_FLAGS_H
#define GLIA_APP_FEATURE_FLAGS_H
#include <map>
#include <string>
namespace glia::app {
class FeatureFlags {
public:
    static std::map<std::string, bool> flags();
};
}
#endif
