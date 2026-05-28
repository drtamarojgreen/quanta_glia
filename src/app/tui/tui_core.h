#ifndef GLIA_APP_TUI_CORE_H
#define GLIA_APP_TUI_CORE_H

#include <vector>
#include <string>
#include <map>
#include <deque>
#include <chrono>

namespace glia::app {

enum class TuiMode { Normal, Palette, Command, Vim };
enum class TuiView {
    Dashboard, Workspace, Notifications, AuditTrail, Explorer,
    Plugins, Changelog, Tenant, History, Diff, Profile,
    Settings, Analytics, Alerts, Privacy, Benchmarks
};

struct Toast {
    std::string message;
    std::string severity;
};

struct AuditEntry {
    std::string action;
    std::string timestamp;
};

struct TuiContext {
    std::vector<std::string> bookmarks;
    std::vector<std::string> clipboardRing;
    std::vector<std::string> commandHistory;
    std::vector<TuiView> viewHistory;
    std::vector<AuditEntry> auditTrail;
    int historyIndex = -1;
    int paneWidth = 80;
    bool denseMode = false;
    int selectionIndex = 0;
    int tutorialStep = 0;
    std::vector<double> healthHistory;
    std::vector<std::string> selectedRows;
    std::map<std::string, std::string> savedQueries;
    std::string pin = "1234";
    bool absoluteTime = true;
    bool caseSensitive = false;
    bool telemetry = false;
};

class UndoRedoManager {
public:
    void push(const std::string& state);
    std::string undo();
    std::string redo();
private:
    std::vector<std::string> m_stack;
    int m_ptr = -1;
};

class NotificationCenter {
public:
    void add(const std::string& msg, const std::string& sev = "INFO");
    const std::deque<Toast>& history() const { return m_history; }
private:
    std::deque<Toast> m_history;
};

class MacroRecorder {
public:
    void start();
    void stop();
    void record(const std::string& cmd);
    const std::vector<std::string>& getMacro() const { return m_macro; }
private:
    bool m_active = false;
    std::vector<std::string> m_macro;
};

}

#endif
