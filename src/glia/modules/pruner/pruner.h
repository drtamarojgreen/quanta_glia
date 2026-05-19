#ifndef SDD_PRUNER_H
#define SDD_PRUNER_H

#include <QString>
#include <QStringList>
#include <QSet>
#include <QMap>
#include <QDateTime>
#include "glia/core/glia_config.h"

struct RepoPruneRecord {
    QString name;
    QString path;
    QDateTime lastActivity;
    float usageScore;
    float redundancyScore;
    float ethicsRiskScore;
};

class Pruner {
public:
    static QSet<QString> buildDependencyMap(const QString &knowledgeBase);
    static float calculateCompositeScore(const RepoPruneRecord &repo, const QMap<QString, float> &weights);
    static QString makePruningDecision(float score, const QMap<QString, float> &thresholds);
    static bool performPruning(const QString &repoPath, const QString &decision, const QString &archivePath, bool dryRun = false);
};

#endif // SDD_PRUNER_H
