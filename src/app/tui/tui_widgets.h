#ifndef GLIA_APP_TUI_WIDGETS_H
#define GLIA_APP_TUI_WIDGETS_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace glia::app {

class Widget {
public:
    virtual ~Widget() = default;
    virtual void render() = 0;
    virtual std::string title() const = 0;
};

class WindowManager {
public:
    void addWidget(std::shared_ptr<Widget> w);
    void clear();
    void renderAll();
private:
    std::vector<std::shared_ptr<Widget>> m_widgets;
};

struct Theme {
    std::string header;
    std::string bar;
    std::string text;
};

class ThemeManager {
public:
    ThemeManager();
    void setTheme(const std::string& name);
    const Theme& current() const;
    std::string currentName() const;
private:
    std::map<std::string, Theme> m_themes;
    std::string m_currentTheme;
};

}

#endif
