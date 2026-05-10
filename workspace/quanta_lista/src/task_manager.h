#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <vector>
#include <memory>
#include "task.h"

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    void addTask(const Task& task);
    void removeTask(int id);
    void updateTask(const Task& task);
    std::vector<Task> getTasks() const;
    void clear();

private:
    std::vector<Task> m_tasks;
    int m_nextId;
};

#endif // TASK_MANAGER_H
