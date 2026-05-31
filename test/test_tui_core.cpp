#include <cassert>
#include <iostream>
#include "app/status_cmd.h"

using namespace glia::app;

void test_undo_redo() {
    UndoRedoManager mgr;
    mgr.push("state1");
    mgr.push("state2");
    assert(mgr.undo() == "state1");
    assert(mgr.redo() == "state2");
    mgr.push("state3");
    assert(mgr.undo() == "state2");
    std::cout << "test_undo_redo passed" << std::endl;
}

void test_key_mapper() {
    KeyMapper mapper;
    assert(mapper.getAction("q") == "quit");
    assert(mapper.getAction("1") == "view_dashboard");
    mapper.remap("x", "custom");
    assert(mapper.getAction("x") == "custom");
    std::cout << "test_key_mapper passed" << std::endl;
}

void test_macro_recorder() {
    MacroRecorder rec;
    rec.start();
    rec.record("cmd1");
    rec.record("cmd2");
    rec.stop();
    rec.record("cmd3");
    auto macro = rec.getMacro();
    assert(macro.size() == 2);
    assert(macro[0] == "cmd1");
    assert(macro[1] == "cmd2");
    std::cout << "test_macro_recorder passed" << std::endl;
}

int main() {
    test_undo_redo();
    test_key_mapper();
    test_macro_recorder();
    return 0;
}
