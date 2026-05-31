#include "tui_core.h"

namespace glia::app {

void UndoRedoManager::push(const std::string& state) {
    if (m_ptr < (int)m_stack.size() - 1) {
        m_stack.erase(m_stack.begin() + m_ptr + 1, m_stack.end());
    }
    m_stack.push_back(state);
    m_ptr++;
}

std::string UndoRedoManager::undo() {
    return (m_ptr > 0) ? m_stack[--m_ptr] : "";
}

std::string UndoRedoManager::redo() {
    return (m_ptr < (int)m_stack.size() - 1) ? m_stack[++m_ptr] : "";
}

void NotificationCenter::add(const std::string& msg, const std::string& sev) {
    m_history.push_back({msg, sev});
    if (m_history.size() > 50) m_history.pop_front();
}

void MacroRecorder::start() {
    m_active = true;
    m_macro.clear();
}

void MacroRecorder::stop() {
    m_active = false;
}

void MacroRecorder::record(const std::string& cmd) {
    if (m_active) m_macro.push_back(cmd);
}

}
