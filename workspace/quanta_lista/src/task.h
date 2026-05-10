#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>

struct Task {
    int id;
    QString title;
    QString description;
    bool completed;
    QDateTime dueDate;

    Task() : id(0), completed(false) {}
};

#endif // TASK_H
