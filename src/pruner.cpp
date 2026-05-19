#include "pruner.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <algorithm>

QSet<QString> Pruner::buildDependencyMap(const QString &knowledgeBase) {
    QSet<QString> dependencies;
    QStringList manifestFiles = {"requirements.txt", "package.json", "go.mod", "pom.xml"};
    QDir kb(knowledgeBase);
    QStringList repoNames = kb.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QDirIterator it(knowledgeBase, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QFileInfo info = it.fileInfo();
        if (manifestFiles.contains(info.fileName())) {
            QFile file(info.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();
                for (const QString &name : repoNames) {
                    if (content.contains(name)) {
                        dependencies.insert(name);
                    }
                }
                file.close();
            }
        }
    }
    return dependencies;
}

float Pruner::calculateCompositeScore(const RepoPruneRecord &repo, const QMap<QString, float> &weights) {
    qint64 ageDays = repo.lastActivity.daysTo(QDateTime::currentDateTime());
    float maxAgeForNorm = 365.0f;
    float normalizedAge = std::min((float)ageDays / maxAgeForNorm, 1.0f);

    float usageWeight = weights.value("usage", 0.5f);
    float ageWeight = weights.value("age", 0.2f);
    float redundancyWeight = weights.value("redundancy", 0.2f);
    float ethicsWeight = weights.value("ethics_risk", 0.1f);

    return (1.0f - repo.usageScore) * usageWeight +
           normalizedAge * ageWeight +
           repo.redundancyScore * redundancyWeight +
           repo.ethicsRiskScore * ethicsWeight;
}

QString Pruner::makePruningDecision(float score, const QMap<QString, float> &thresholds) {
    if (score >= thresholds.value("delete", 0.9f)) return "DELETE";
    if (score >= thresholds.value("archive", 0.7f)) return "ARCHIVE";
    if (score >= thresholds.value("review", 0.5f)) return "REVIEW";
    return "KEEP";
}

bool Pruner::performPruning(const QString &repoPath, const QString &decision, const QString &archivePath, bool dryRun) {
    if (decision == "KEEP" || decision == "REVIEW") return true;

    QFileInfo info(repoPath);
    QString repoName = info.fileName();

    if (decision == "ARCHIVE") {
        QDir().mkpath(archivePath);
        QString dest = QDir(archivePath).absoluteFilePath(repoName);
        if (dryRun) {
            qInfo() << "[DRY RUN] Would archive" << repoName << "to" << dest;
            return true;
        }
        return QDir().rename(repoPath, dest);
    }

    if (decision == "DELETE") {
        if (dryRun) {
            qInfo() << "[DRY RUN] Would DELETE" << repoName;
            return true;
        }
        return QDir(repoPath).removeRecursively();
    }

    return false;
}
