#include "cli/tui.h"
#include <cassert>
#include <iostream>

void test_tui_canvas_basic() {
    std::cout << "Running test_tui_canvas_basic...\n";
    glia::cli::TuiCanvas canvas(10, 10);
    canvas.setChar(5, 5, 'X');
    canvas.drawString(0, 0, "Hello");
    
    // Inspect buffer logic
    const auto& buffer = canvas.getBuffer();
    assert(buffer[5 * 10 + 5] == 'X');
    assert(buffer[0] == 'H');
    assert(buffer[1] == 'e');
    assert(buffer[2] == 'l');
    assert(buffer[3] == 'l');
    assert(buffer[4] == 'o');
    std::cout << "  - Canvas buffer logic ... PASSED\n";
}

void test_tui_box_drawing() {
    std::cout << "Running test_tui_box_drawing...\n";
    glia::cli::TuiCanvas canvas(10, 10);
    canvas.drawBox(0, 0, 3, 3, "");
    
    const auto& buffer = canvas.getBuffer();
    // Check corners
    assert(buffer[0 * 10 + 0] == '+'); // Top-left
    assert(buffer[0 * 10 + 2] == '+'); // Top-right
    assert(buffer[2 * 10 + 0] == '+'); // Bottom-left
    assert(buffer[2 * 10 + 2] == '+'); // Bottom-right
    
    // Check edges
    assert(buffer[0 * 10 + 1] == '-'); // Top edge
    assert(buffer[1 * 10 + 0] == '|'); // Left edge
    
    std::cout << "  - Box drawing logic ... PASSED\n";
}

void run_tui_tests() {
    test_tui_canvas_basic();
    test_tui_box_drawing();
}
