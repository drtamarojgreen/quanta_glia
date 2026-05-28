#include "tui_input.h"

namespace glia::app {

KeyMapper::KeyMapper() {
    m_bindings["q"] = "quit";
    m_bindings["1"] = "view_dashboard";
    m_bindings["2"] = "view_workspace";
    m_bindings["3"] = "view_notifications";
    m_bindings["p"] = "open_palette";
    m_bindings[":"] = "open_palette";
    m_bindings["t"] = "view_tenant";
    m_bindings["T"] = "switch_theme";
    m_bindings["u"] = "undo";
    m_bindings["r"] = "redo";
}

void KeyMapper::remap(const std::string& key, const std::string& action) {
    m_bindings[key] = action;
}

std::string KeyMapper::getAction(const std::string& key) const {
    return m_bindings.count(key) ? m_bindings.at(key) : "";
}

}
