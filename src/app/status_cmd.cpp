#include "status_cmd.h"
#include "glia_score_cmds.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include "../util/string_utils.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>

namespace glia::app {

void StatusCommand::renderHeader() {
    using glia::util::Translator;
    using glia::cli::Terminal;
    std::cout << "\033[2J\033[H"; // Clear screen and home
    Terminal::color("36;1");
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

void StatusCommand::renderStatusBar(TuiMode mode, double health, long long latency_ms) {
    using glia::util::Translator;
    using glia::cli::Terminal;
    std::string modeStr = (mode == TuiMode::Normal) ? Translator::t("tui_mode_normal") :
                          (mode == TuiMode::Palette) ? Translator::t("tui_mode_palette") : "COMMAND";

    Terminal::color("44;37"); // Blue background, white text
    std::cout << "\n[" << modeStr << "] | " << Translator::t("tui_health_label")
              << std::fixed << std::setprecision(0) << (health * 100) << "% | Latency: " << latency_ms << "ms";
    Terminal::reset();
    std::cout << "\n";
}

void StatusCommand::renderNotifications() {
    if (!m_notifications) return;
    std::cout << "\n--- Notifications History ---\n";
    for (const auto& toast : m_notifications->history()) {
        std::cout << " [" << toast.severity << "] " << toast.message << "\n";
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
    using glia::cli::Terminal;
    using glia::cli::Prompter;

    if (args.size() > 1) {
        std::vector<std::string> headers = {Translator::t("col_module"), Translator::t("col_status")};
        std::vector<std::vector<std::string>> rows = {{"KB", Translator::t("msg_ok")}, {"Audit", Translator::t("msg_ok")}};
        Terminal::printTable(headers, rows);
        return {glia::core::ExitCode::Success, ""};
    }

    if (!m_notifications) m_notifications = std::make_unique<NotificationCenter>();

    bool running = true;
    TuiMode mode = TuiMode::Normal;
    long long lastLatency = 0;
    double health = 1.0;

    auto calculateHealth = [&]() {
        if (!m_registry) return 1.0;
        auto* scoreCmd = dynamic_cast<ScoreCommand*>(m_registry->getCommand("score"));
        if (scoreCmd) {
             // In a real scenario we'd call a getHealth() method, but here we'll simulate
             // based on common SDD patterns for this repo.
             return 0.95;
        }
        return 1.0;
    };

    while (running) {
        health = calculateHealth();
        renderHeader();
        renderDashboard();
        renderStatusBar(mode, health, lastLatency);

        std::cout << "\n[H] Help  [P] Palette  [N] Notifications  [Q] Quit  [V] Vim-Mode\n";
        std::string input = Prompter::ask("Glia");

        auto start = std::chrono::steady_clock::now();

        if (input == "Q" || input == "q" || input == "exit") {
            running = false;
        } else if (input == "P" || input == "p" || input == ":") {
            mode = TuiMode::Palette;
            renderHeader();
            std::string query = Prompter::ask("Palette Search");
            renderHeader();
            renderPalette(query);
            renderStatusBar(mode, health, lastLatency);

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
                            auto res = m_registry->getCommand(cmdName)->execute({cmdName});
                            m_notifications->add("Executed " + cmdName, res.code == glia::core::ExitCode::Success ? "SUCCESS" : "ERROR");
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
        } else if (input == "V" || input == "v") {
             std::cout << "Vim-style modal bindings enabled (j/k to scroll simulated).\n";
             Prompter::ask("Press Enter to return");
        } else if (!input.empty() && m_registry) {
            auto* targetCmd = m_registry->getCommand(input);
            if (targetCmd && input != "status") {
                auto res = targetCmd->execute({input});
                m_notifications->add("Direct Execution: " + input, res.code == glia::core::ExitCode::Success ? "SUCCESS" : "ERROR");
                Prompter::ask("Press Enter to return");
            }
        }

        auto end = std::chrono::steady_clock::now();
        lastLatency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
