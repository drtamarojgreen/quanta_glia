#include "lifecycle.h"
#include <iostream>

namespace glia::app {
void Lifecycle::preRun() { /* Validation etc */ }
void Lifecycle::postRun(const glia::core::CommandResult& result) {
    if (result.code != glia::core::ExitCode::Success) {
        std::cerr << "Error: " << result.userMessage << "\n";
        if (!result.detailMessage.empty()) std::cerr << "Details: " << result.detailMessage << "\n";
        for (const auto& hint : result.hints) std::cerr << "Hint: " << hint << "\n";
    }
}
}
