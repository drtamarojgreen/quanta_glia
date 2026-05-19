#ifndef SDD_HARVESTER_H
#define SDD_HARVESTER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include "glia/core/glia_config.h"

class Harvester {
public:
    static QString cloneRepo(const QString &repoUrl, const QString &cacheDir);
    static QMap<QString, QString> extractText(const QString &repoPath, const GliaConfig &config);
    static bool persistToKnowledgeBase(const QString &repoName, const QMap<QString, QString> &content, const QString &kbPath);
};

#endif // SDD_HARVESTER_H
