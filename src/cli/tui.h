#ifndef GLIA_CLI_TUI_H
#define GLIA_CLI_TUI_H

#include <string>
#include <vector>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace glia::cli {

struct Point { int x, y; };
struct Size { int w, h; };

class TuiCanvas {
public:
    TuiCanvas(int w, int h) : width(w), height(h), buffer(w * h, ' ') {}

    void setChar(int x, int y, char c) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            buffer[y * width + x] = c;
        }
    }

    void drawString(int x, int y, const std::string& s) {
        for (size_t i = 0; i < s.length(); ++i) {
            setChar(x + i, y, s[i]);
        }
    }

    void drawBox(int x, int y, int w, int h, const std::string& title = "") {
        for (int i = 0; i < w; ++i) {
            setChar(x + i, y, '-');
            setChar(x + i, y + h - 1, '-');
        }
        for (int i = 0; i < h; ++i) {
            setChar(x, y + i, '|');
            setChar(x + w - 1, y + i, '|');
        }
        setChar(x, y, '+');
        setChar(x + w - 1, y, '+');
        setChar(x, y + h - 1, '+');
        setChar(x + w - 1, y + h - 1, '+');
        
        if (!title.empty()) {
            drawString(x + 2, y, " " + title + " ");
        }
    }

    void render() {
        std::cout << "\033[H"; // Cursor to home
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                std::cout << buffer[y * width + x];
            }
            std::cout << "\n";
        }
        std::cout << std::flush;
    }

    void clear() {
        std::fill(buffer.begin(), buffer.end(), ' ');
    }

    const std::vector<char>& getBuffer() const { return buffer; }

private:
    int width, height;
    std::vector<char> buffer;
};

class RawTerminal {
public:
    static void enable() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        termios raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        std::cout << "\033[?25l"; // Hide cursor
        std::cout << "\033[2J";   // Clear screen
    }

    static void disable() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        std::cout << "\033[?25h"; // Show cursor
        std::cout << "\033[2J";   // Clear screen
    }

private:
    static inline termios orig_termios;
};

}

#endif
