#ifndef GLIA_CORE_STATE_H
#define GLIA_CORE_STATE_H
#include <string>
#include <map>
#include <vector>

namespace glia::core {
class State {
public:
    static void save(const std::string& path);
    static void load(const std::string& path);
    static void set(const std::string& key, const std::string& val);
private:
    static std::map<std::string, std::string>& data();
};
}
#endif
