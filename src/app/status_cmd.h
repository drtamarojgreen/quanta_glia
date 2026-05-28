#ifndef GLIA_APP_STATUS_CMD_H
#define GLIA_APP_STATUS_CMD_H
#include "command.h"
#include <deque>
#include <map>
#include <string>
#include <memory>

namespace glia::app {
enum class TuiMode { Normal, Palette, Command };
enum class TuiView { Dashboard, Workspace, Notifications };

struct Theme {
    std::string header;
    std::string bar;
    std::string text;
};

class ThemeManager {
public:
    ThemeManager() {
        m_themes["default"] = {"36;1", "44;37", "0"};
        m_themes["high-contrast"] = {"37;1;40", "30;47", "0"};
        m_currentTheme = "default";
    }
    void setTheme(const std::string& name) { if (m_themes.count(name)) m_currentTheme = name; }
    const Theme& current() const { return m_themes.at(m_currentTheme); }
    std::string currentName() const { return m_currentTheme; }
private:
    std::map<std::string, Theme> m_themes;
    std::string m_currentTheme;
};

struct Toast {
    std::string message;
    std::string severity;
};

class NotificationCenter {
public:
    void add(const std::string& msg, const std::string& sev = "INFO") {
        m_history.push_back({msg, sev});
        if (m_history.size() > 50) m_history.pop_front();
    }
    const std::deque<Toast>& history() const { return m_history; }
private:
    std::deque<Toast> m_history;
};

class StatusCommand : public Command {
public:
    std::string name() const override { return "status"; }
    std::string description() const override { return "Show system status dashboard"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
private:
    void renderHeader();
    void renderDashboard();
    void renderStatusBar(TuiMode mode, double health, long long latency_ms);
    void renderPalette(const std::string& query);
    void renderNotifications();
    void renderTabBar(TuiView activeView);
    void renderWorkspaceView();
    void renderBreadcrumbs(TuiView view, TuiMode mode);
    void renderHelpPane(TuiView view);
    std::unique_ptr<NotificationCenter> m_notifications;
    std::unique_ptr<ThemeManager> m_themes;
};
}
#endif
