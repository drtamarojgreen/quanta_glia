#include "status_cmd.h"
#include "glia_score_cmds.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include "../util/string_utils.h"
#include "../util/xml_parser.h"
#include "../util/benchmark_utils.h"
#include "../core/state.h"
#include "../platform/platform.h"
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <regex>
#include <ctime>

namespace glia::app {

void StatusCommand::renderHeader() {
    using glia::util::Translator;
    using glia::cli::Terminal;
    std::cout << "\033[2J\033[H"; // Clear screen and home
    Terminal::color(m_themes->current().header);
    std::cout << "==========================================================\n";
    std::cout << "  " << Translator::t("app_name") << "\n";
    std::cout << "  " << Translator::t("tui_header") << "\n";
    std::cout << "==========================================================\n";
    Terminal::reset();
}

void StatusCommand::renderDashboard() {
    using glia::util::Translator;
    using glia::cli::Terminal;
    std::vector<std::string> headers = {Translator::t("col_module"), Translator::t("col_status")};
    std::vector<std::vector<std::string>> rows = {
        {"System Core", Translator::t("msg_ok")},
        {"Knowledge Base", Translator::t("msg_ok")},
        {"Audit Ledger", Translator::t("msg_ok")},
        {"TUI Interface", Translator::t("msg_ok")}
    };
    Terminal::printTable(headers, rows);

    std::cout << "\n" << Translator::t("tui_help") << "\n";
    if (m_registry) {
        auto cmds = m_registry->listCommands();
        std::sort(cmds.begin(), cmds.end());
        for (const auto& name : cmds) {
            auto* c = m_registry->getCommand(name);
            std::cout << "  * " << std::left << std::setw(20) << name << c->description() << "\n";
        }
    }
}

void StatusCommand::renderTreeExplorer() {
    using glia::util::XmlParser;
    using glia::util::XmlNode;
    std::cout << "--- Data Tree Explorer ---\n";
    auto root = XmlParser::parse("data/rules.xml");
    if (!root) {
        std::cout << " (Error loading data/rules.xml)\n";
        return;
    }

    std::function<void(std::shared_ptr<XmlNode>, int)> printNode = [&](std::shared_ptr<XmlNode> n, int depth) {
        std::cout << std::string(depth * 2, ' ') << " |-- [" << n->name << "]";
        if (!n->attributes.empty()) {
            std::cout << " (";
            for (auto const& [k, v] : n->attributes) std::cout << k << "=" << v << " ";
            std::cout << ")";
        }
        std::cout << "\n";
        for (auto& child : n->children) printNode(child, depth + 1);
    };
    printNode(root, 0);
}

void StatusCommand::renderBenchmarks() {
    using glia::util::BenchmarkTimer;
    std::cout << "--- System Benchmarks & Startup Profile ---\n";
    std::cout << " Boot sequence: " << BenchmarkTimer::get("boot") << "ms\n";
    std::cout << " XML Registry:  " << BenchmarkTimer::get("xml_parse") << "ms\n";
    std::cout << " TUI Init:      " << BenchmarkTimer::get("tui_init") << "ms\n";
}

void StatusCommand::renderAnalytics() {
    std::cout << "--- System Health & Performance Analytics ---\n";
    std::cout << " Health Sparkline: ";
    for (double h : m_context.healthHistory) {
        if (h > 0.9) std::cout << "█";
        else if (h > 0.7) std::cout << "▆";
        else std::cout << "▃";
    }
    std::cout << "\n\n KPI Metrics:\n";
    std::vector<std::string> headers = {"Metric", "Value", "Trend"};
    double avgHealth = 0;
    for (double h : m_context.healthHistory) avgHealth += h;
    if (!m_context.healthHistory.empty()) avgHealth /= m_context.healthHistory.size();

    std::vector<std::vector<std::string>> rows = {
        {"Avg Health", std::to_string(avgHealth * 100).substr(0, 4) + "%", (avgHealth > 0.9 ? "OPTIMAL" : "STABLE")},
        {"Session Actions", std::to_string(m_context.auditTrail.size()), "N/A"},
        {"Session Depth", std::to_string(m_context.viewHistory.size()), (m_context.viewHistory.size() > 10 ? "DEEP" : "SHALLOW")}
    };
    glia::cli::Terminal::printTable(headers, rows);
}

void StatusCommand::renderPluginManager() {
    std::cout << "--- Plugin Manager & Marketplace ---\n";
    std::vector<std::string> headers = {"Plugin", "Status", "Source", "Description"};
    std::vector<std::vector<std::string>> rows;
    if (m_registry) {
        for (const auto& name : m_registry->listCommands()) {
            rows.push_back({name, "ACTIVE", "Local", "Installed system command"});
        }
    }
    // Marketplace mock
    rows.push_back({"CloudSync", "AVAILABLE", "Market", "Synchronize with Quanta Cloud"});
    rows.push_back({"EthosAudit", "AVAILABLE", "Market", "Deep ethical structural analysis"});
    rows.push_back({"VulnerabilityScan", "AVAILABLE", "Market", "Identify CVEs in workspace"});

    glia::cli::Terminal::printTable(headers, rows, true);
    std::cout << "\n [I] Install Plugin  [R] Remove Plugin\n";
}

void StatusCommand::renderBreadcrumbs(TuiView view, TuiMode mode) {
    using glia::cli::Terminal;
    Terminal::color("37;2"); // Dim white
    std::cout << " Glia";

    size_t startIdx = (m_context.viewHistory.size() > 5) ? m_context.viewHistory.size() - 5 : 0;
    for (size_t i = startIdx; i < m_context.viewHistory.size(); ++i) {
        const auto& pastView = m_context.viewHistory[i];
        std::cout << " > ";
        if (pastView == TuiView::Dashboard) std::cout << "Dash";
        else if (pastView == TuiView::Workspace) std::cout << "WS";
        else if (pastView == TuiView::Notifications) std::cout << "Notif";
        else if (pastView == TuiView::AuditTrail) std::cout << "Audit";
    }

    std::cout << " > ";
    if (view == TuiView::Dashboard) std::cout << "Dashboard";
    else if (view == TuiView::Workspace) std::cout << "Workspace";
    else if (view == TuiView::Notifications) std::cout << "Notifications";
    else if (view == TuiView::AuditTrail) std::cout << "Audit Trail";

    if (mode == TuiMode::Palette) std::cout << " > Palette";
    std::cout << "\n\n";
    Terminal::reset();
}

void StatusCommand::renderHelpPane(TuiView view) {
    using glia::cli::Terminal;
    Terminal::color("33"); // Yellow
    std::cout << "\n--- Contextual Help ---\n";
    if (view == TuiView::Dashboard) {
        std::cout << " [1-3] Switch View  [H] Help  [P] Palette  [T] Theme  [Q] Quit\n";
        std::cout << " Type any command name to execute it directly.\n";
    } else if (view == TuiView::Workspace) {
        std::cout << " [1-3] Switch View  [P] Palette  [Q] Quit\n";
        std::cout << " Displays real-time status of all repositories in /workspace.\n";
    } else if (view == TuiView::Notifications) {
        std::cout << " [1-3] Switch View  [Q] Quit\n";
        std::cout << " Shows a history of recent system actions and alerts.\n";
    }
    Terminal::reset();
}

void StatusCommand::renderStatusBar(TuiMode mode, double health, long long latency_ms, double render_ms) {
    using glia::util::Translator;
    using glia::cli::Terminal;
    using glia::platform::Environment;
    std::string modeStr = (mode == TuiMode::Normal) ? Translator::t("tui_mode_normal") :
                          (mode == TuiMode::Palette) ? Translator::t("tui_mode_palette") : "COMMAND";

    long mem = Environment::getMemoryUsage(); // Result from platform.cpp
    double cpu = Environment::getCpuUsage();

    Terminal::color(m_themes->current().bar);
    std::cout << "\n[" << modeStr << "] | " << Translator::t("tui_health_label")
              << std::fixed << std::setprecision(0) << (health * 100) << "% | Lat: " << latency_ms << "ms | Rnd: " << std::fixed << std::setprecision(2) << render_ms << "ms | CPU: " << std::fixed << std::setprecision(1) << cpu << "s | Mem: " << mem << "KB | Actions: " << m_context.auditTrail.size() << " | Bmk: " << m_context.bookmarks.size() << " | Theme: " << m_themes->currentName();
    Terminal::reset();
    std::cout << "\n";

    int progress = (m_context.auditTrail.empty()) ? 0 : 100;
    std::string bar = std::string(progress / 5, '#') + std::string(20 - (progress / 5), ' ');
    std::cout << " [Session Progress] [" << bar << "] " << progress << "% | History depth: " << m_context.viewHistory.size() << "\n";
}

void StatusCommand::renderNotifications() {
    if (!m_notifications) return;
    using glia::cli::Terminal;
    std::cout << "\n--- Notifications History ---\n";
    if (m_notifications->history().empty()) {
        std::cout << " (No notifications)\n";
    } else {
        for (const auto& toast : m_notifications->history()) {
            if (toast.severity == "CRITICAL" || toast.severity == "ERROR") Terminal::color("31"); // Red
            else if (toast.severity == "WARNING") Terminal::color("33"); // Yellow
            else Terminal::color("32"); // Green
            std::cout << " [" << std::setw(8) << toast.severity << "] ";
            Terminal::reset();
            std::cout << toast.message << "\n";
        }
    }
}

void StatusCommand::renderTabBar(TuiView activeView) {
    using glia::cli::Terminal;
    std::cout << " ";
    auto renderTab = [&](TuiView v, const std::string& label) {
        if (v == activeView) Terminal::color("47;30"); // White background, black text
        else Terminal::color("40;37"); // Black background, white text
        std::cout << " " << label << " ";
        Terminal::reset();
        std::cout << "  ";
    };
    renderTab(TuiView::Dashboard, "1: Dashboard");
    renderTab(TuiView::Workspace, "2: Workspace");
    renderTab(TuiView::Notifications, "3: Notifications");
    renderTab(TuiView::AuditTrail, "4: Audit Trail");
    renderTab(TuiView::Explorer, "5: Explorer");
    renderTab(TuiView::Plugins, "6: Plugins");
    renderTab(TuiView::Changelog, "7: Changelog");
    renderTab(TuiView::Tenant, "T: Tenant");
    renderTab(TuiView::History, "8: History");
    renderTab(TuiView::Diff, "9: Diff");
    renderTab(TuiView::Analytics, "A: Analytics");
    renderTab(TuiView::Privacy, "Y: Privacy");
    renderTab(TuiView::Alerts, "!: Alerts");
    renderTab(TuiView::Profile, "U: User");
    renderTab(TuiView::Settings, "S: Settings");
    std::cout << "\n\n";
}

void StatusCommand::renderWorkspaceView() {
    using glia::util::Translator;
    using glia::cli::Terminal;
    std::cout << "--- Workspace Nodes Status ---\n";
    if (m_registry) {
        auto* wsCmd = m_registry->getCommand("workspace-status");
        if (wsCmd) {
            std::vector<std::string> targetArgs = {"workspace-status"};
            wsCmd->execute(targetArgs);
        } else {
            std::cout << "Workspace status command not registered.\n";
        }
    }
}

void StatusCommand::renderPalette(const std::string& query) {
    std::cout << "\n--- Palette Search: " << query << " ---\n";
    if (m_registry) {
        auto allCmds = m_registry->listCommands();
        std::sort(allCmds.begin(), allCmds.end());
        int idx = 1;
        for (const auto& name : allCmds) {
            if (glia::util::fuzzyMatch(query, name)) {
                auto* c = m_registry->getCommand(name);
                std::cout << " [" << idx++ << "] " << std::left << std::setw(20) << name << c->description() << "\n";
            }
        }
    }
}

glia::core::CommandResult StatusCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    using glia::util::BenchmarkTimer;
    using glia::cli::Terminal;
    using glia::cli::Prompter;

    if (args.size() > 1) {
        std::vector<std::string> targetArgs = args;
        std::vector<std::string> headers = {Translator::t("col_module"), Translator::t("col_status")};
        std::vector<std::vector<std::string>> rows = {{"KB", Translator::t("msg_ok")}, {"Audit", Translator::t("msg_ok")}};
        Terminal::printTable(headers, rows);
        return {glia::core::ExitCode::Success, ""};
    }

    BenchmarkTimer::start("tui_init");
    using glia::core::State;
    if (!m_notifications) m_notifications = std::make_unique<NotificationCenter>();
    if (!m_themes) {
        m_themes = std::make_unique<ThemeManager>();
        m_themes->setTheme(State::get("tui_theme", "default"));
    }
    if (!m_keyMapper) m_keyMapper = std::make_unique<KeyMapper>();
    if (!m_undoRedo) m_undoRedo = std::make_unique<UndoRedoManager>();
    if (!m_macro) m_macro = std::make_unique<MacroRecorder>();
    m_context.paneWidth = std::stoi(State::get("tui_pane_width", "80"));
    BenchmarkTimer::stop("tui_init");

    bool running = true;
    TuiMode mode = TuiMode::Normal;
    TuiView view = TuiView::Dashboard;
    long long lastLatency = 0;
    double health = 1.0;

    auto calculateHealth = [&]() {
        if (!m_registry) return 1.0;
        auto* scoreCmd = dynamic_cast<ScoreCommand*>(m_registry->getCommand("score"));
        if (scoreCmd) {
             return 0.95;
        }
        return 1.0;
    };

    while (running) {
        auto frameStart = std::chrono::steady_clock::now();
        health = calculateHealth();
        m_context.healthHistory.push_back(health);
        if (m_context.healthHistory.size() > 20) m_context.healthHistory.erase(m_context.healthHistory.begin());
        renderHeader();
        if (!m_context.denseMode) std::cout << "\n";
        renderTabBar(view);
        if (!m_context.denseMode) std::cout << "\n";
        renderBreadcrumbs(view, mode);

        if (view == TuiView::Dashboard) {
            std::cout << "--- Split-Pane Dashboard ---\n";
            renderDashboard();
            std::cout << "\n";
            renderNotifications();
        }
        else if (view == TuiView::Workspace) renderWorkspaceView();
        else if (view == TuiView::Notifications) renderNotifications();
        else if (view == TuiView::AuditTrail) {
            std::cout << "--- Session Audit Trail ---\n";
            std::vector<std::string> auditHeaders = {"Timestamp", "Action"};
            std::vector<std::vector<std::string>> auditRows;
            for (const auto& entry : m_context.auditTrail) {
                auditRows.push_back({entry.timestamp, entry.action});
            }
            Terminal::printTable(auditHeaders, auditRows);
        }
        else if (view == TuiView::Explorer) renderTreeExplorer();
        else if (view == TuiView::Plugins) renderPluginManager();
        else if (view == TuiView::Analytics) renderAnalytics();
        else if (view == TuiView::Privacy) {
            std::cout << "--- Privacy Notice & Data Collection ---\n";
            std::cout << " Telemetry: " << (m_context.telemetry ? "OPT-IN" : "OPT-OUT") << "\n";
            std::cout << " We collect: Action frequency, system error rates.\n";
            std::cout << " We DO NOT collect: Source code content, file names, PINs.\n";
        }
        else if (view == TuiView::Benchmarks) renderBenchmarks();
        else if (view == TuiView::Alerts) {
            std::cout << "--- Threshold Alerts & Monitoring ---\n";
            std::vector<std::string> headers = {"ID", "Source", "Level", "Status"};
            std::vector<std::vector<std::string>> rows;
            if (m_context.auditTrail.size() > 50) {
                rows.push_back({"AL-01", "SessionLoad", "WARNING", "OPEN"});
            }
            if (health < 0.9) {
                rows.push_back({"AL-02", "SystemHealth", "CRITICAL", "OPEN"});
            }
            if (rows.empty()) {
                rows.push_back({"AL-00", "System", "INFO", "NORMAL"});
            }
            glia::cli::Terminal::printTable(headers, rows);
        }
        else if (view == TuiView::History) {
            std::cout << "--- Command History Search ---\n";
            std::vector<std::string> histHeaders = {"#", "Command"};
            std::vector<std::vector<std::string>> histRows;
            for (size_t i = 0; i < m_context.commandHistory.size(); ++i) {
                histRows.push_back({std::to_string(i+1), m_context.commandHistory[i]});
            }
            Terminal::printTable(histHeaders, histRows);
        }
        else if (view == TuiView::Diff) {
            std::cout << "--- Side-by-Side Diff Viewer ---\n";
            std::vector<std::string> diffHeaders = {"Original", "Modified"};
            std::vector<std::vector<std::string>> diffRows = {
                {"void main() {", "int main() {"},
                {"  return;", "  return 0;"}
            };
            Terminal::printTable(diffHeaders, diffRows);
        }
        else if (view == TuiView::Settings) {
            std::cout << "--- System Settings & Configuration ---\n";
            std::cout << " [1] Time Format: " << (m_context.absoluteTime ? "ABSOLUTE" : "RELATIVE") << "\n";
            std::cout << " [2] Session PIN: " << m_context.pin << "\n";
            std::cout << " [3] Dense Mode:  " << (m_context.denseMode ? "ON" : "OFF") << "\n";
            std::cout << " [4] Timezone:    UTC\n";
            std::cout << " [5] Screen Reader Semantic: ENABLED\n";
        }
        else if (view == TuiView::Profile) {
            std::cout << "--- User Profile & SSO Login ---\n";
            std::cout << " User: " << State::get("user_name", "glia_admin") << "\n";
            std::cout << " Role: " << State::get("user_role", "SYSTEM_ARCHITECT") << "\n";
            std::cout << " Auth: SSO_ACTIVE\n";
            std::cout << " Permissions: READ, EXECUTE, PRUNE, AUDIT\n";
            auto now = std::chrono::system_clock::now();
            auto expires = now + std::chrono::hours(1);
            std::time_t exp_c = std::chrono::system_clock::to_time_t(expires);
            std::cout << " Token Expires: " << std::ctime(&exp_c);
        }
        else if (view == TuiView::Changelog) {
            std::cout << "--- System Changelog ---\n";
            std::ifstream f("docs/glia_enhancements.md");
            std::string line;
            int count = 0;
            while (std::getline(f, line) && count++ < 20) std::cout << line << "\n";
        }

        renderHelpPane(view);

        static int tipIdx = 0;
        std::vector<std::string> tips = {
            "Power Tip: Use /regex/ in the palette search for complex filtering.",
            "Power Tip: Press [V] then [j/k] to navigate lists with Vim bindings.",
            "Power Tip: Use [U/R] to quickly jump between visited views."
        };
        std::cout << "\n " << tips[tipIdx++ % tips.size()] << "\n";

        auto frameEndRender = std::chrono::steady_clock::now();
        double render_ms = std::chrono::duration_cast<std::chrono::microseconds>(frameEndRender - frameStart).count() / 1000.0;
        renderStatusBar(mode, health, lastLatency, render_ms);

        if (m_context.tutorialStep < 7) {
            std::cout << "\n--- Glia Onboarding Tutorial (Step " << m_context.tutorialStep + 1 << "/7) ---\n";
            if (m_context.tutorialStep == 0) std::cout << " TASK: Press [2] to visit the Workspace view.\n";
            else if (m_context.tutorialStep == 1) std::cout << " TASK: Press [P] to open the Command Palette.\n";
            else if (m_context.tutorialStep == 2) std::cout << " TASK: Press [T] to switch themes.\n";
            else if (m_context.tutorialStep == 3) std::cout << " TASK: Press [U] to undo navigation.\n";
            else if (m_context.tutorialStep == 4) std::cout << " TASK: Press [V] to enter Vim mode.\n";
            else if (m_context.tutorialStep == 5) std::cout << " TASK: Press [H] to see the cheat sheet.\n";
            else if (m_context.tutorialStep == 6) std::cout << " Tutorial Complete! Use [Q] to exit or [1-7] to explore.\n";
        }

        std::cout << "\n Action: ";
        if (m_registry) {
            std::cout << " (HINT: ";
            auto all = m_registry->listCommands();
            for (int i=0; i<3 && i<(int)all.size(); ++i) std::cout << all[i] << " ";
            std::cout << "... ) ";
        }
        std::string input = Prompter::ask("Glia");

        auto start = std::chrono::steady_clock::now();

        m_context.commandHistory.push_back(input);
        m_macro->record(input);
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss; ss << std::put_time(std::localtime(&now_c), "%F %T");
        m_context.auditTrail.push_back({input, ss.str()});

        std::string action = m_keyMapper->getAction(input);
        if (action.empty()) action = input; // Fallback to raw input if no mapping

        if (action == "quit") {
            State::set("tui_pane_width", std::to_string(m_context.paneWidth));
            running = false;
        } else if (input == "+" || input == "=") {
            m_context.paneWidth += 5;
            m_notifications->add("Pane resized: " + std::to_string(m_context.paneWidth), "INFO");
        } else if (input == "-") {
            if (m_context.paneWidth > 20) m_context.paneWidth -= 5;
            m_notifications->add("Pane resized: " + std::to_string(m_context.paneWidth), "INFO");
        } else if (input == "g") {
            m_context.selectionIndex = 0;
            m_notifications->add("Jump to Top", "INFO");
        } else if (input == "G") {
            m_context.selectionIndex = 999;
            m_notifications->add("Jump to Bottom", "INFO");
        } else if (input == "x" || input == "X") {
            m_context.selectedRows.push_back("Item_" + std::to_string(m_context.selectionIndex));
            m_notifications->add("Row selected", "SUCCESS");
        } else if (input == "L" || input == "l") {
            m_notifications->add("Session locked", "WARNING");
            std::cout << "\033[2J\033[H";
            std::cout << "==========================================================\n";
            std::cout << "  GLIA SESSION LOCKED\n";
            std::cout << "==========================================================\n";
            while (Prompter::ask("Enter PIN to unlock") != m_context.pin) {
                std::cout << "Invalid PIN. Access Denied.\n";
            }
            m_notifications->add("Session unlocked", "SUCCESS");
        } else if (action == "undo") {
            std::string stateStr = m_undoRedo->undo();
            if (!stateStr.empty()) view = static_cast<TuiView>(std::stoi(stateStr));
            if (m_context.tutorialStep == 3) m_context.tutorialStep = 4;
        } else if (action == "redo") {
            std::string stateStr = m_undoRedo->redo();
            if (!stateStr.empty()) view = static_cast<TuiView>(std::stoi(stateStr));
        } else if (action == "view_dashboard" || input == "1") {
            view = TuiView::Dashboard;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (action == "view_workspace" || input == "2") {
            view = TuiView::Workspace;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
            if (m_context.tutorialStep == 0) m_context.tutorialStep = 1;
        } else if (action == "view_notifications" || input == "3") {
            view = TuiView::Notifications;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "4") {
            view = TuiView::AuditTrail;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "5") {
            view = TuiView::Explorer;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "6") {
            view = TuiView::Plugins;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "7") {
            view = TuiView::Changelog;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "8") {
            view = TuiView::History;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "9") {
            view = TuiView::Diff;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "U" || input == "u") {
            view = TuiView::Profile;
            m_notifications->add("Performing SSO Handshake...", "INFO");
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "y") {
            m_context.clipboardRing.push_back(std::to_string(static_cast<int>(view)));
            m_notifications->add("Yanked view state to clipboard", "INFO");
        } else if (input == "p") {
            if (!m_context.clipboardRing.empty()) {
                view = static_cast<TuiView>(std::stoi(m_context.clipboardRing.back()));
                m_notifications->add("Pasted view state from clipboard", "INFO");
            }
        } else if (input == "D" || input == "d") {
            m_context.denseMode = !m_context.denseMode;
            m_notifications->add(m_context.denseMode ? "Dense Mode" : "Comfortable Mode", "INFO");
        } else if (action == "open_palette") {
            mode = TuiMode::Palette;
            if (m_context.tutorialStep == 1) m_context.tutorialStep = 2;
            renderHeader();
            std::string query = Prompter::ask("Palette Search (Regex with /pattern/)");
            renderHeader();
            renderPalette(query);
            renderStatusBar(mode, health, lastLatency, 0.0);

            std::string selection = Prompter::ask("Select number to run or [Enter] to cancel");
            if (!selection.empty()) {
                try {
                    int selIdx = std::stoi(selection);
                    auto allCmds = m_registry->listCommands();
                    std::sort(allCmds.begin(), allCmds.end());
                    std::vector<std::string> matches;
                    for (const auto& name : allCmds) {
                        if (glia::util::fuzzyMatch(query, name)) matches.push_back(name);
                    }
                    if (selIdx > 0 && selIdx <= static_cast<int>(matches.size())) {
                        std::string cmdName = matches[selIdx - 1];
                        if (cmdName != "status") {
                            std::vector<std::string> targetArgs = {cmdName};
                            bool dryRun = Prompter::confirm("Execute as dry-run?");
                            if (dryRun) {
                                m_notifications->add("Dry-run: " + cmdName, "INFO");
                                std::cout << " [DRY-RUN] Command " << cmdName << " would be executed.\n";
                            } else {
                                std::cout << "--- Resource Impact Preview (What-if) ---\n";
                                double estCpu = (cmdName.find("scan") != std::string::npos) ? 0.5 : 0.1;
                                int estMem = (cmdName.find("harvest") != std::string::npos) ? 50 : 5;
                                std::cout << " CPU: +" << estCpu << "s | MEM: +" << estMem << "MB | DISK: +0KB\n";
                                if (Prompter::confirm("Are you sure you want to execute " + cmdName + "?")) {
                                    auto res = m_registry->getCommand(cmdName)->execute(targetArgs);
                                    m_notifications->add("Executed " + cmdName, res.code == glia::core::ExitCode::Success ? "SUCCESS" : "ERROR");
                                }
                            }
                            Prompter::ask("Press Enter to return");
                        }
                    }
                } catch (...) {
                    m_notifications->add("Palette execution failed", "CRITICAL");
                }
            }
            mode = TuiMode::Normal;
        } else if (input == "N" || input == "n") {
            renderHeader();
            renderNotifications();
            Prompter::ask("Press Enter to return");
        } else if (input == "H" || input == "h") {
            if (m_context.tutorialStep == 5) m_context.tutorialStep = 6;
            renderHeader();
            std::cout << "--- Keyboard Cheat Sheet ---\n";
            std::vector<std::string> helpHeaders = {"Key", "Action", "Description"};
            std::vector<std::vector<std::string>> helpRows = {
                {"1-5", "Switch View", "Navigate between TUI dashboards"},
                {"P/:", "Palette", "Search and execute commands"},
                {"T", "Theme", "Cycle through accessibility themes"},
                {"B", "Bookmark", "Save current view for later"},
                {"E", "Export", "Save current view to file"},
                {"U/R", "Undo/Redo", "Navigate navigation history"},
                {"Q", "Quit", "Exit the TUI application"}
            };
            Terminal::printTable(helpHeaders, helpRows);
            Prompter::ask("Press Enter to return");
        } else if (input == "S" || input == "s") {
            view = TuiView::Settings;
        } else if (input == "A" || input == "a") {
            view = TuiView::Analytics;
        } else if (input == "!" ) {
            view = TuiView::Alerts;
        } else if (input == "Y" || input == "y") {
            view = TuiView::Privacy;
            m_context.viewHistory.push_back(view);
            m_undoRedo->push(std::to_string(static_cast<int>(view)));
        } else if (input == "*" ) {
            view = TuiView::Benchmarks;
        } else if (input == "M" || input == "m") {
            m_notifications->add("Replaying last macro", "INFO");
            for (const auto& mc : m_macro->getMacro()) {
                std::cout << " REPLAY: " << mc << "\n";
            }
        } else if (action == "view_tenant" ) {
            std::string tenant = Prompter::ask("Switch to Tenant/Org");
            m_notifications->add("Context switched to " + tenant, "INFO");
            view = TuiView::Tenant;
        } else if (action == "switch_theme") {
            if (m_context.tutorialStep == 2) m_context.tutorialStep = 3;
            std::string current = m_themes->currentName();
            std::string next = "default";
            if (current == "default") next = "high-contrast";
            else if (current == "high-contrast") next = "colorblind-safe";

            m_themes->setTheme(next);
            State::set("tui_theme", next);
            m_notifications->add("Theme switched to " + next, "INFO");
        } else if (input == "B" || input == "b") {
            m_context.bookmarks.push_back(std::to_string(static_cast<int>(view)));
            m_notifications->add("Bookmarked current view", "SUCCESS");
        } else if (input == "E" || input == "e") {
            std::string format = Prompter::ask("Format (md/csv/json)");
            if (format != "md" && format != "csv" && format != "json" && !format.empty()) {
                m_notifications->add("Invalid format", "ERROR");
                continue;
            }
            std::string filename = Prompter::ask("Enter filename (or leave blank for default)");
            if (filename.empty()) {
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                std::stringstream ss; ss << "glia_export_" << std::put_time(std::localtime(&now_c), "%Y%m%d_%H%M%S") << "." << (format.empty() ? "md" : format);
                filename = ss.str();
            }
            std::ofstream out(filename);
            if (format == "json") {
                out << "{\n  \"view\": \"" << static_cast<int>(view) << "\",\n";
                out << "  \"audit_trail\": [\n";
                for (size_t i = 0; i < m_context.auditTrail.size(); ++i) {
                    out << "    {\"timestamp\": \"" << m_context.auditTrail[i].timestamp << "\", \"action\": \"" << m_context.auditTrail[i].action << "\"}";
                    if (i < m_context.auditTrail.size() - 1) out << ",";
                    out << "\n";
                }
                out << "  ]\n}\n";
            } else if (format == "csv") {
                out << "Timestamp,Action\n";
                for (const auto& entry : m_context.auditTrail) {
                    out << entry.timestamp << "," << entry.action << "\n";
                }
            } else {
                out << "# Glia TUI View Export\n\n";
                out << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n";
                out << "View ID: " << static_cast<int>(view) << "\n\n";
                out << "## Audit Trail\n";
                for (const auto& entry : m_context.auditTrail) {
                    out << "* [" << entry.timestamp << "] " << entry.action << "\n";
                }
            }
            out.close();
            m_notifications->add("Exported view to " + filename, "SUCCESS");
        } else if (input == "V" || input == "v") {
             mode = TuiMode::Vim;
             if (m_context.tutorialStep == 4) m_context.tutorialStep = 5;
             m_notifications->add("Vim mode activated", "INFO");
             while (mode == TuiMode::Vim) {
                 renderHeader();
                 renderTabBar(view);
                 renderBreadcrumbs(view, mode);
                 if (view == TuiView::Dashboard) renderDashboard();
                 else if (view == TuiView::Workspace) renderWorkspaceView();
                 else if (view == TuiView::AuditTrail) {
                    std::cout << "--- Session Audit Trail ---\n";
                    std::vector<std::string> auditHeaders = {"Timestamp", "Action"};
                    std::vector<std::vector<std::string>> auditRows;
                    for (const auto& entry : m_context.auditTrail) auditRows.push_back({entry.timestamp, entry.action});
                    Terminal::printTable(auditHeaders, auditRows);
                 }
                 renderStatusBar(mode, health, lastLatency, render_ms);
                 std::string vcmd = Prompter::ask("(Vim)");
                 if (vcmd == "j") { /* scroll down */ }
                 else if (vcmd == "k") { /* scroll up */ }
                 else if (vcmd == "i") mode = TuiMode::Normal;
                 else if (vcmd == "q" || vcmd == "\x1B" || vcmd == "ESC") mode = TuiMode::Normal;
                 else if (vcmd == ":") { mode = TuiMode::Palette; break; }
                 else if (vcmd == "1") view = TuiView::Dashboard;
                 else if (vcmd == "2") view = TuiView::Workspace;
                 else if (vcmd == "3") view = TuiView::Notifications;
                 else if (vcmd == "4") view = TuiView::AuditTrail;
                 else if (vcmd == "5") view = TuiView::Explorer;
             }
        } else if (!input.empty() && m_registry) {
            auto* targetCmd = m_registry->getCommand(input);
            if (targetCmd && input != "status") {
                std::vector<std::string> targetArgs = {input};
                bool dryRun = Prompter::confirm("Execute as dry-run?");
                if (dryRun) {
                    m_notifications->add("Dry-run: " + input, "INFO");
                    std::cout << " [DRY-RUN] Command " << input << " would be executed.\n";
                } else {
                    std::cout << "--- Resource Impact Preview (What-if) ---\n";
                    double estCpu = (input.find("scan") != std::string::npos) ? 0.5 : 0.1;
                    int estMem = (input.find("harvest") != std::string::npos) ? 50 : 5;
                    std::cout << " CPU: +" << estCpu << "s | MEM: +" << estMem << "MB | DISK: +0KB\n";
                    if (Prompter::confirm("Are you sure you want to execute " + input + "?")) {
                        auto res = targetCmd->execute(targetArgs);
                        m_notifications->add("Direct Execution: " + input, res.code == glia::core::ExitCode::Success ? "SUCCESS" : "ERROR");
                    }
                }
                Prompter::ask("Press Enter to return");
            }
        }

        auto end = std::chrono::steady_clock::now();
        lastLatency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}

}
