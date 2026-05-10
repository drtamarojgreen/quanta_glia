#include "mainwindow.h"
#include "config_manager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    m_taskTitleEdit = new QLineEdit(this);
    m_taskTitleEdit->setPlaceholderText("Enter task title...");
    layout->addWidget(m_taskTitleEdit);

    m_addButton = new QPushButton("Add Task", this);
    layout->addWidget(m_addButton);

    m_taskListWidget = new QListWidget(this);
    layout->addWidget(m_taskListWidget);

    m_removeButton = new QPushButton("Remove Selected Task", this);
    layout->addWidget(m_removeButton);

    QHBoxLayout *xmlLayout = new QHBoxLayout();
    m_saveXmlButton = new QPushButton("Save to XML", this);
    m_loadXmlButton = new QPushButton("Load from XML", this);
    xmlLayout->addWidget(m_saveXmlButton);
    xmlLayout->addWidget(m_loadXmlButton);
    layout->addLayout(xmlLayout);

    QHBoxLayout *jsonLayout = new QHBoxLayout();
    m_saveJsonButton = new QPushButton("Save to JSON", this);
    m_loadJsonButton = new QPushButton("Load from JSON", this);
    jsonLayout->addWidget(m_saveJsonButton);
    jsonLayout->addWidget(m_loadJsonButton);
    layout->addLayout(jsonLayout);

    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::handleAddTask);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::handleRemoveTask);
    connect(m_saveXmlButton, &QPushButton::clicked, this, &MainWindow::handleSaveXml);
    connect(m_loadXmlButton, &QPushButton::clicked, this, &MainWindow::handleLoadXml);
    connect(m_saveJsonButton, &QPushButton::clicked, this, &MainWindow::handleSaveJson);
    connect(m_loadJsonButton, &QPushButton::clicked, this, &MainWindow::handleLoadJson);

    setWindowTitle("Quanta Lista");
    resize(400, 500);
}

MainWindow::~MainWindow() {}

void MainWindow::handleAddTask() {
    QString title = m_taskTitleEdit->text();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Task title cannot be empty.");
        return;
    }

    Task task;
    task.title = title;
    task.dueDate = QDateTime::currentDateTime();
    m_taskManager.addTask(task);
    m_taskTitleEdit->clear();
    refreshTaskList();
}

void MainWindow::handleRemoveTask() {
    QListWidgetItem *item = m_taskListWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a task to remove.");
        return;
    }

    int id = item->data(Qt::UserRole).toInt();
    m_taskManager.removeTask(id);
    refreshTaskList();
}

void MainWindow::handleSaveXml() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save XML", "", "XML Files (*.xml)");
    if (!filePath.isEmpty()) {
        if (ConfigManager::saveToXml(filePath, m_taskManager.getTasks())) {
            QMessageBox::information(this, "Success", "Saved to XML successfully.");
        }
    }
}

void MainWindow::handleLoadXml() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load XML", "", "XML Files (*.xml)");
    if (!filePath.isEmpty()) {
        std::vector<Task> tasks;
        if (ConfigManager::loadFromXml(filePath, tasks)) {
            m_taskManager.clear();
            for (const auto& task : tasks) {
                m_taskManager.addTask(task);
            }
            refreshTaskList();
        }
    }
}

void MainWindow::handleSaveJson() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save JSON", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (ConfigManager::saveToJson(filePath, m_taskManager.getTasks())) {
            QMessageBox::information(this, "Success", "Saved to JSON successfully.");
        }
    }
}

void MainWindow::handleLoadJson() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load JSON", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        std::vector<Task> tasks;
        if (ConfigManager::loadFromJson(filePath, tasks)) {
            m_taskManager.clear();
            for (const auto& task : tasks) {
                m_taskManager.addTask(task);
            }
            refreshTaskList();
        }
    }
}

void MainWindow::refreshTaskList() {
    m_taskListWidget->clear();
    auto tasks = m_taskManager.getTasks();
    for (const auto& task : tasks) {
        QListWidgetItem *item = new QListWidgetItem(task.title, m_taskListWidget);
        item->setData(Qt::UserRole, task.id);
    }
}
