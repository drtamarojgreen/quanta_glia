#include "glia_config_manager.h"
#include <QFile>
#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

bool GliaConfigManager::saveToXml(const GliaConfig &config, const QString &filePath) {
    QDomDocument doc;
    QDomElement root = doc.createElement("GliaConfig");
    doc.appendChild(root);

    QDomElement kb = doc.createElement("knowledge_base");
    kb.appendChild(doc.createTextNode(config.knowledgeBase()));
    root.appendChild(kb);

    QDomElement rc = doc.createElement("repo_cache");
    rc.appendChild(doc.createTextNode(config.repoCache()));
    root.appendChild(rc);

    QDomElement mr = doc.createElement("max_repos");
    mr.appendChild(doc.createTextNode(QString::number(config.maxRepos())));
    root.appendChild(mr);

    QDomElement topics = doc.createElement("target_topics");
    for (const QString &topic : config.targetTopics()) {
        QDomElement t = doc.createElement("topic");
        t.appendChild(doc.createTextNode(topic));
        topics.appendChild(t);
    }
    root.appendChild(topics);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << doc.toString();
    file.close();
    return true;
}

bool GliaConfigManager::loadFromXml(GliaConfig &config, const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != "GliaConfig")
        return false;

    config.setKnowledgeBase(root.firstChildElement("knowledge_base").text());
    config.setRepoCache(root.firstChildElement("repo_cache").text());
    config.setMaxRepos(root.firstChildElement("max_repos").text().toInt());

    QStringList topics;
    QDomElement topicsElem = root.firstChildElement("target_topics");
    QDomElement topicElem = topicsElem.firstChildElement("topic");
    while (!topicElem.isNull()) {
        topics << topicElem.text();
        topicElem = topicElem.nextSiblingElement("topic");
    }
    config.setTargetTopics(topics);

    return true;
}

bool GliaConfigManager::saveToJson(const GliaConfig &config, const QString &filePath) {
    QJsonObject root;
    root["knowledge_base"] = config.knowledgeBase();
    root["repo_cache"] = config.repoCache();
    root["max_repos"] = config.maxRepos();

    QJsonArray topics;
    for (const QString &topic : config.targetTopics()) {
        topics.append(topic);
    }
    root["target_topics"] = topics;

    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(doc.toJson());
    file.close();
    return true;
}

bool GliaConfigManager::loadFromJson(GliaConfig &config, const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject())
        return false;

    QJsonObject root = doc.object();
    config.setKnowledgeBase(root["knowledge_base"].toString());
    config.setRepoCache(root["repo_cache"].toString());
    config.setMaxRepos(root["max_repos"].toInt());

    QStringList topics;
    QJsonArray topicsArray = root["target_topics"].toArray();
    for (const QJsonValue &val : topicsArray) {
        topics << val.toString();
    }
    config.setTargetTopics(topics);

    return true;
}
