#include "task_manager.h"
#include <algorithm>

TaskManager::TaskManager() : m_nextId(1) {}

TaskManager::~TaskManager() {}

void TaskManager::addTask(const Task& task) {
    Task newTask = task;
    newTask.id = m_nextId++;
    m_tasks.push_back(newTask);
}

void TaskManager::removeTask(int id) {
    m_tasks.erase(std::remove_if(m_tasks.begin(), m_tasks.end(),
        [id](const Task& t) { return t.id == id; }), m_tasks.end());
}

void TaskManager::updateTask(const Task& task) {
    for (auto& t : m_tasks) {
        if (t.id == task.id) {
            t = task;
            break;
        }
    }
}

std::vector<Task> TaskManager::getTasks() const {
    return m_tasks;
}

void TaskManager::clear() {
    m_tasks.clear();
    m_nextId = 1;
}
