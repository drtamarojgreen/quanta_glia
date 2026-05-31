#ifndef GLIA_APP_TUI_INPUT_H
#define GLIA_APP_TUI_INPUT_H

#include <string>
#include <map>

namespace glia::app {

class KeyMapper {
public:
    KeyMapper();
    void remap(const std::string& key, const std::string& action);
    std::string getAction(const std::string& key) const;
private:
    std::map<std::string, std::string> m_bindings;
};

}

#endif
