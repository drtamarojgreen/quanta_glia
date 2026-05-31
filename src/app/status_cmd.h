#ifndef GLIA_APP_STATUS_CMD_H
#define GLIA_APP_STATUS_CMD_H

#include "command.h"
#include "tui/tui_core.h"
#include "tui/tui_widgets.h"
#include "tui/tui_input.h"
#include <memory>

namespace glia::app {

class StatusCommand : public Command {
public:
    std::string name() const override { return "status"; }
    std::string description() const override { return "Show system status dashboard"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;

private:
    void renderHeader();
    void renderDashboard();
    void renderStatusBar(TuiMode mode, double health, long long latency_ms, double render_ms);
    void renderPalette(const std::string& query);
    void renderNotifications();
    void renderTabBar(TuiView activeView);
    void renderWorkspaceView();
    void renderBreadcrumbs(TuiView view, TuiMode mode);
    void renderHelpPane(TuiView view);
    void renderTreeExplorer();
    void renderPluginManager();
    void renderAnalytics();
    void renderBenchmarks();

    std::unique_ptr<NotificationCenter> m_notifications;
    std::unique_ptr<ThemeManager> m_themes;
    std::unique_ptr<KeyMapper> m_keyMapper;
    std::unique_ptr<UndoRedoManager> m_undoRedo;
    std::unique_ptr<MacroRecorder> m_macro;
    std::unique_ptr<WindowManager> m_windowManager;
    TuiContext m_context;
};

}

#endif
