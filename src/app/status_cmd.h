#ifndef GLIA_APP_STATUS_CMD_H
#define GLIA_APP_STATUS_CMD_H
#include "command.h"
#include <deque>

namespace glia::app {
enum class TuiMode { Normal, Palette, Command };

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
    std::unique_ptr<NotificationCenter> m_notifications;
};
}
#endif
