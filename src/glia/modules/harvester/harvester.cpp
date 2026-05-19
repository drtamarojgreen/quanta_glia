#include "glia/modules/harvester/harvester.h"
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

QString Harvester::cloneRepo(const QString &repoUrl, const QString &cacheDir) {
    QString repoName = QFileInfo(repoUrl).fileName().replace(".git", "");
    QDir cache(cacheDir);
    QString destPath = cache.absoluteFilePath(repoName);

    if (QDir(destPath).exists()) {
        qInfo() << "Repo" << repoName << "already in cache. Skipping.";
        return destPath;
    }

    if (QDir(repoUrl).exists()) {
        qInfo() << "Local directory detected at" << repoUrl << ". Copying to cache.";
        auto copyRecursively = [](auto &&self, const QString &src, const QString &dest) -> bool {
            QDir srcDir(src);
            if (!srcDir.exists()) return false;
            if (!QDir().mkpath(dest)) return false;

            for (const QString &dir : srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (!self(self, src + "/" + dir, dest + "/" + dir)) return false;
            }
            for (const QString &file : srcDir.entryList(QDir::Files)) {
                if (!QFile::copy(src + "/" + file, dest + "/" + file)) return false;
            }
            return true;
        };
        if (copyRecursively(copyRecursively, repoUrl, destPath)) return destPath;
        return "";
    }

    qInfo() << "Cloning repo:" << repoUrl;
    QProcess process;
    process.start("git", {"clone", "--depth=1", repoUrl, destPath});
    if (process.waitForFinished() && process.exitCode() == 0) {
        return destPath;
    }

    qCritical() << "Failed to clone repo:" << repoUrl;
    return "";
}

QMap<QString, QString> Harvester::extractText(const QString &repoPath, const GliaConfig &config) {
    QMap<QString, QString> extractedContent;
    QStringList targetTopics = config.targetTopics();
    QStringList searchPaths = config.searchPaths();
    int maxDepth = config.maxDepth();
    int maxLines = config.maxLinesPerFile();

    QDir baseRepoDir(repoPath);
    QString absRepoPath = baseRepoDir.absolutePath();

    for (const QString &sp : searchPaths) {
        QString relSp = sp;
        if (relSp.startsWith("/")) relSp.remove(0, 1);
        QString absSearchPath = baseRepoDir.absoluteFilePath(relSp);

        if (!QDir(absSearchPath).exists()) {
            continue;
        }

        QDirIterator it(absSearchPath, QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString fpath = it.next();
            QFileInfo info(fpath);

            QString relPathToRepo = baseRepoDir.relativeFilePath(info.absoluteFilePath());
            QString relPathToSearch = QDir(absSearchPath).relativeFilePath(info.absoluteFilePath());

            int depth = (relPathToSearch == "." || relPathToSearch.isEmpty()) ? 0 : relPathToSearch.count('/') + 1;
            if (depth > maxDepth) continue;

            bool matchesTopic = false;
            for (const QString &topic : targetTopics) {
                if (info.fileName().contains(topic, Qt::CaseInsensitive)) {
                    matchesTopic = true;
                    break;
                }
            }

            if (matchesTopic) {
                QFile file(info.absoluteFilePath());
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    QString content;
                    if (maxLines > 0) {
                        for (int i = 0; i < maxLines && !in.atEnd(); ++i) {
                            content += in.readLine() + "\n";
                        }
                    } else {
                        content = in.readAll();
                    }
                    extractedContent[relPathToRepo] = content;
                    file.close();
                }
            }
        }
    }
    return extractedContent;
}

bool Harvester::persistToKnowledgeBase(const QString &repoName, const QMap<QString, QString> &content, const QString &kbPath) {
    QDir kbDir(kbPath);
    QString baseTargetDir = kbDir.absoluteFilePath(repoName);
    for (auto it = content.begin(); it != content.end(); ++it) {
        QFileInfo targetFileInfo(QDir(baseTargetDir).absoluteFilePath(it.key()));
        QDir().mkpath(targetFileInfo.absolutePath());
        QFile file(targetFileInfo.absoluteFilePath());
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << it.value();
            file.close();
        } else {
            return false;
        }
    }
    return true;
}
