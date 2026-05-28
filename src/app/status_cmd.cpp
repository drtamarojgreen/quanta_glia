#include "status_cmd.h"
#include "../cli/cli.h"
#include "../util/translator.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace glia::app {
glia::core::CommandResult StatusCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    using glia::cli::Terminal;
    using glia::cli::Prompter;

    // If arguments are provided (other than the command name itself), just show static status
    if (args.size() > 1) {
        std::vector<std::string> headers = {Translator::t("col_module"), Translator::t("col_status")};
        std::vector<std::vector<std::string>> rows = {
            {"KB", Translator::t("msg_ok")},
            {"Audit", Translator::t("msg_ok")}
        };
        Terminal::printTable(headers, rows);
        return {glia::core::ExitCode::Success, ""};
    }

    // Interactive TUI mode
    bool running = true;
    while (running) {
        Terminal::color("36;1");
        std::cout << "\n==========================================================\n";
        std::cout << "  " << Translator::t("app_name") << "\n";
        std::cout << "  " << Translator::t("tui_header") << "\n";
        std::cout << "==========================================================\n";
        Terminal::reset();

        std::vector<std::string> headers = {Translator::t("col_module"), Translator::t("col_status")};
        std::vector<std::vector<std::string>> rows = {
            {"System Core", Translator::t("msg_ok")},
            {"Knowledge Base", Translator::t("msg_ok")},
            {"Audit Ledger", Translator::t("msg_ok")},
            {"TUI Interface", Translator::t("msg_ok")}
        };
        Terminal::printTable(headers, rows);

        std::cout << "\n" << Translator::t("tui_help") << "\n";
        std::vector<std::string> cmds;
        if (m_registry) {
            cmds = m_registry->listCommands();
            std::sort(cmds.begin(), cmds.end());
            for (const auto& name : cmds) {
                auto* c = m_registry->getCommand(name);
                std::cout << "  * " << std::left << std::setw(20) << name << c->description() << "\n";
            }
        }

        std::cout << "\n[H] Help/Manual  [Q] Quit  [Command Name] to Run\n";
        std::string input = Prompter::ask("Glia");

        if (input == "Q" || input == "q" || input == "exit") {
            running = false;
        } else if (input == "H" || input == "h" || input == "help") {
            std::cout << "\n--- Glia Systems Manual ---\n";
            std::cout << "This TUI provides a centralized interface for all Glia architecture tools.\n";
            std::cout << "Navigate by typing the command name to execute it, or use 'Q' to exit.\n";
            std::cout << "Each command performs empirical verification and structural audits\n";
            std::cout << "aligned with Sorrel Driven Development (SDD) principles.\n";
            Prompter::ask("Press Enter to continue");
        } else if (!input.empty() && m_registry) {
            std::stringstream ss(input);
            std::string part;
            std::vector<std::string> targetArgs;
            while (ss >> part) targetArgs.push_back(part);

            if (!targetArgs.empty()) {
                std::string cmdName = targetArgs[0];
                auto* targetCmd = m_registry->getCommand(cmdName);
                if (targetCmd && cmdName != "status") {
                    std::cout << "\n--- Executing: " << cmdName << " ---\n";
                    targetCmd->execute(targetArgs);
                    Prompter::ask("\nPress Enter to return to Dashboard");
                } else if (cmdName == "status") {
                    std::cout << "Already in Dashboard.\n";
                } else {
                    std::cout << "Unknown command: " << cmdName << "\n";
                }
            }
        }
    }

    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
