#include "trigger.h"
#include <fstream>
#include <iostream>

namespace glia::trigger {
void TriggerEngine::record(const HistoryEntry& entry) {
    std::ofstream out("command.history", std::ios::app);
    if (out.is_open()) {
        out << entry.command << " | " << entry.exitCode << " | " << entry.durationMs << "\n";
    }
}

std::vector<std::string> TriggerEngine::analyze() {
    std::vector<std::string> suggestions;
    std::ifstream in("command.history");
    std::string line;
    int failures = 0;
    while (std::getline(in, line)) {
        if (line.find(" | 0 |") == std::string::npos) failures++;
    }
    if (failures > 3) suggestions.push_back("High failure rate detected. Check your configuration.");
    return suggestions;
}
}
