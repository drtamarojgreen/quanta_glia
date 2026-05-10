#include "config_manager.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

bool ConfigManager::saveToXml(const QString& filePath, const std::vector<Task>& tasks) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("tasks");

    for (const auto& task : tasks) {
        writer.writeStartElement("task");
        writer.writeAttribute("id", QString::number(task.id));
        writer.writeTextElement("title", task.title);
        writer.writeTextElement("description", task.description);
        writer.writeTextElement("completed", task.completed ? "true" : "false");
        writer.writeTextElement("dueDate", task.dueDate.toString(Qt::ISODate));
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    return true;
}

bool ConfigManager::loadFromXml(const QString& filePath, std::vector<Task>& tasks) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for reading:" << filePath;
        return false;
    }

    QXmlStreamReader reader(&file);
    tasks.clear();

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "task") {
                Task task;
                task.id = reader.attributes().value("id").toInt();
                while (!reader.atEnd() && !(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "task")) {
                    reader.readNext();
                    if (reader.tokenType() == QXmlStreamReader::StartElement) {
                        if (reader.name() == "title") task.title = reader.readElementText();
                        else if (reader.name() == "description") task.description = reader.readElementText();
                        else if (reader.name() == "completed") task.completed = (reader.readElementText() == "true");
                        else if (reader.name() == "dueDate") task.dueDate = QDateTime::fromString(reader.readElementText(), Qt::ISODate);
                    }
                }
                tasks.push_back(task);
            }
        }
    }

    if (reader.hasError()) {
        qWarning() << "XML error:" << reader.errorString();
        return false;
    }

    return true;
}

bool ConfigManager::saveToJson(const QString& filePath, const std::vector<Task>& tasks) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QJsonArray taskArray;
    for (const auto& task : tasks) {
        QJsonObject taskObject;
        taskObject["id"] = task.id;
        taskObject["title"] = task.title;
        taskObject["description"] = task.description;
        taskObject["completed"] = task.completed;
        taskObject["dueDate"] = task.dueDate.toString(Qt::ISODate);
        taskArray.append(taskObject);
    }

    QJsonDocument doc(taskArray);
    file.write(doc.toJson());
    return true;
}

bool ConfigManager::loadFromJson(const QString& filePath, std::vector<Task>& tasks) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qWarning() << "Invalid JSON data";
        return false;
    }

    tasks.clear();
    QJsonArray taskArray = doc.array();
    for (int i = 0; i < taskArray.size(); ++i) {
        QJsonObject taskObject = taskArray[i].toObject();
        Task task;
        task.id = taskObject["id"].toInt();
        task.title = taskObject["title"].toString();
        task.description = taskObject["description"].toString();
        task.completed = taskObject["completed"].toBool();
        task.dueDate = QDateTime::fromString(taskObject["dueDate"].toString(), Qt::ISODate);
        tasks.push_back(task);
    }

    return true;
}
