#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include "task_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleAddTask();
    void handleRemoveTask();
    void handleSaveXml();
    void handleLoadXml();
    void handleSaveJson();
    void handleLoadJson();

private:
    void refreshTaskList();

    TaskManager m_taskManager;
    QListWidget *m_taskListWidget;
    QLineEdit *m_taskTitleEdit;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_saveXmlButton;
    QPushButton *m_loadXmlButton;
    QPushButton *m_saveJsonButton;
    QPushButton *m_loadJsonButton;
};

#endif // MAINWINDOW_H
