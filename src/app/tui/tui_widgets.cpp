#include "tui_widgets.h"
#include <iostream>

namespace glia::app {

void WindowManager::addWidget(std::shared_ptr<Widget> w) {
    m_widgets.push_back(w);
}

void WindowManager::clear() {
    m_widgets.clear();
}

void WindowManager::renderAll() {
    for (auto& w : m_widgets) {
        std::cout << "--- " << w->title() << " ---\n";
        w->render();
        std::cout << "\n";
    }
}

ThemeManager::ThemeManager() {
    m_themes["default"] = {"36;1", "44;37", "0"};
    m_themes["high-contrast"] = {"37;1;40", "30;47", "0"};
    m_themes["colorblind-safe"] = {"34;1", "46;30", "0"};
    m_currentTheme = "default";
}

void ThemeManager::setTheme(const std::string& name) {
    if (m_themes.count(name)) {
        m_currentTheme = name;
    }
}

const Theme& ThemeManager::current() const {
    return m_themes.at(m_currentTheme);
}

std::string ThemeManager::currentName() const {
    return m_currentTheme;
}

}
