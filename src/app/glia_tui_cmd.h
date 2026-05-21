#ifndef GLIA_APP_GLIA_TUI_CMD_H
#define GLIA_APP_GLIA_TUI_CMD_H

#include "command.h"
#include "../cli/tui.h"
#include <vector>
#include <memory>

namespace glia::app {

struct Node {
    std::string name;
    int x, y;
    std::vector<int> connections; // Indices of dependent nodes
};

class TuiCommand : public Command {
public:
    std::string name() const override { return "tui"; }
    std::string description() const override { return "Launch interactive workflow editor"; }
    
    glia::core::CommandResult execute(const std::vector<std::string>& args) override {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int width = w.ws_col;
        int height = w.ws_row;

        glia::cli::RawTerminal::enable();
        glia::cli::TuiCanvas canvas(width, height);

        std::vector<Node> nodes = {
            {"git-fetch", 5, 5, {1}},
            {"git-pull", 20, 5, {2}},
            {"build-all", 35, 5, {}}
        };

        int selectedNode = 0;
        bool running = true;

        while (running) {
            canvas.clear();
            
            // Draw Sidebar
            canvas.drawBox(width - 25, 0, 25, height, "Commands");
            canvas.drawString(width - 23, 2, "1. git-fetch");
            canvas.drawString(width - 23, 3, "2. git-pull");
            canvas.drawString(width - 23, 4, "3. build-all");
            canvas.drawString(width - 23, height - 2, "[Q] Quit");

            // Draw Graph Area
            canvas.drawBox(0, 0, width - 25, height, "Workflow Graph");
            
            for (size_t i = 0; i < nodes.size(); ++i) {
                const auto& node = nodes[i];
                std::string title = (selectedNode == (int)i) ? "> " + node.name + " <" : "[" + node.name + "]";
                canvas.drawBox(node.x, node.y, 15, 3, "");
                canvas.drawString(node.x + 1, node.y + 1, title);

                // Draw connections
                for (int targetIdx : node.connections) {
                    const auto& target = nodes[targetIdx];
                    for (int x = node.x + 15; x < target.x; ++x) {
                        canvas.setChar(x, node.y + 1, '-');
                    }
                    canvas.setChar(target.x - 1, node.y + 1, '>');
                }
            }

            canvas.render();

            char c;
            if (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == 'q' || c == 'Q') running = false;
                if (c == '\033') { // Escape sequence
                    char seq[2];
                    if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                        if (seq[0] == '[') {
                            switch (seq[1]) {
                                case 'C': selectedNode = (selectedNode + 1) % nodes.size(); break; // Right
                                case 'D': selectedNode = (selectedNode - 1 + nodes.size()) % nodes.size(); break; // Left
                            }
                        }
                    }
                }
            }
        }

        glia::cli::RawTerminal::disable();
        return {glia::core::ExitCode::Success, "TUI session ended"};
    }
};

}

#endif
