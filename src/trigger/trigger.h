#ifndef GLIA_TRIGGER_TRIGGER_H
#define GLIA_TRIGGER_TRIGGER_H
#include <string>
#include <vector>

namespace glia::trigger {
struct HistoryEntry {
    std::string command;
    int exitCode;
    long long durationMs;
};

class TriggerEngine {
public:
    static void record(const HistoryEntry& entry);
    static std::vector<std::string> analyze();
};
}
#endif
