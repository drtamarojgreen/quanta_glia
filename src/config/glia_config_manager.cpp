#include "glia/core/glia_config_manager.h"
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

    QDomElement ll = doc.createElement("log_level");
    ll.appendChild(doc.createTextNode(config.logLevel()));
    root.appendChild(ll);

    QDomElement md = doc.createElement("max_depth");
    md.appendChild(doc.createTextNode(QString::number(config.maxDepth())));
    root.appendChild(md);

    QDomElement ml = doc.createElement("max_lines_per_file");
    ml.appendChild(doc.createTextNode(QString::number(config.maxLinesPerFile())));
    root.appendChild(ml);

    QDomElement paths = doc.createElement("search_paths");
    for (const QString &path : config.searchPaths()) {
        QDomElement p = doc.createElement("path");
        p.appendChild(doc.createTextNode(path));
        paths.appendChild(p);
    }
    root.appendChild(paths);

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
    config.setLogLevel(root.firstChildElement("log_level").text());
    config.setMaxDepth(root.firstChildElement("max_depth").text().toInt());
    config.setMaxLinesPerFile(root.firstChildElement("max_lines_per_file").text().toInt());

    QStringList paths;
    QDomElement pathsElem = root.firstChildElement("search_paths");
    QDomElement pathElem = pathsElem.firstChildElement("path");
    while (!pathElem.isNull()) {
        paths << pathElem.text();
        pathElem = pathElem.nextSiblingElement("path");
    }
    config.setSearchPaths(paths);

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
    root["log_level"] = config.logLevel();
    root["max_depth"] = config.maxDepth();
    root["max_lines_per_file"] = config.maxLinesPerFile();

    QJsonArray paths;
    for (const QString &path : config.searchPaths()) paths.append(path);
    root["search_paths"] = paths;

    QJsonArray topics;
    for (const QString &topic : config.targetTopics()) topics.append(topic);
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
    config.setLogLevel(root["log_level"].toString());
    config.setMaxDepth(root["max_depth"].toInt());
    config.setMaxLinesPerFile(root["max_lines_per_file"].toInt());

    QStringList paths;
    QJsonArray pathsArray = root["search_paths"].toArray();
    for (const QJsonValue &val : pathsArray) paths << val.toString();
    config.setSearchPaths(paths);

    QStringList topics;
    QJsonArray topicsArray = root["target_topics"].toArray();
    for (const QJsonValue &val : topicsArray) topics << val.toString();
    config.setTargetTopics(topics);
    return true;
}
