#ifndef GLIA_APP_LIFECYCLE_H
#define GLIA_APP_LIFECYCLE_H
#include "../core/result.h"
#include <string>

namespace glia::app {
class Lifecycle {
public:
    static void preRun();
    static void postRun(const glia::core::CommandResult& result);
};
}
#endif
