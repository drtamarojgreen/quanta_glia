#include "glia/modules/reporting/reporting.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <algorithm>

RepoReportData Reporting::analyzeRepository(const QString &repoPath) {
    RepoReportData data;
    data.repoName = QFileInfo(repoPath).fileName();
    data.repoSize = getRepoSize(repoPath);
    data.numFiles = 0;
    data.numSrcFiles = 0;
    data.numDocFiles = 0;
    data.numTestFiles = 0;
    data.techStack = "Unknown";
    data.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate) + "Z";

    QMap<QString, int> extFreq;
    QStringList srcExts = {".py", ".js", ".java", ".c", ".cpp", ".go", ".rs", ".ts", ".html", ".css"};
    QStringList docExts = {".md", ".txt", ".rst"};

    QDirIterator it(repoPath, QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString fpath = it.next();
        fpath = QFileInfo(fpath).absoluteFilePath();
        QFileInfo info(fpath);
        if (fpath.contains("/.git")) continue;
        data.numFiles++;
        QString relPath = QDir(repoPath).relativeFilePath(fpath).toLower();
        QString ext = "." + info.suffix().toLower();
        QString stem = info.baseName().toLower();
        if (relPath.contains("test")) {
            data.numTestFiles++;
        } else if (docExts.contains(ext) || stem == "license" || stem == "contributing") {
            data.numDocFiles++;
        } else if (srcExts.contains(ext)) {
            data.numSrcFiles++;
            extFreq[ext]++;
        }
    }
    if (!extFreq.isEmpty()) {
        QString bestExt;
        int maxCount = -1;
        for (auto it = extFreq.begin(); it != extFreq.end(); ++it) {
            if (it.value() > maxCount) {
                maxCount = it.value();
                bestExt = it.key();
            }
        }
        data.techStack = bestExt;
    }
    return data;
}

bool Reporting::writeToCsv(const RepoReportData &data, const QString &filename) {
    bool fileExists = QFile::exists(filename);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return false;

    QTextStream out(&file);
    if (!fileExists) {
        out << "repository_name,repository_size,number_files,number_src_files,number_doc_files,number_test_files,tech_stack,timestamp\n";
    }

    auto escapeCsv = [](QString s) {
        if (s.contains(",") || s.contains("\"") || s.contains("\n")) {
            s.replace("\"", "\"\"");
            return "\"" + s + "\"";
        }
        return s;
    };

    out << escapeCsv(data.repoName) << ","
        << data.repoSize << ","
        << data.numFiles << ","
        << data.numSrcFiles << ","
        << data.numDocFiles << ","
        << data.numTestFiles << ","
        << escapeCsv(data.techStack) << ","
        << escapeCsv(data.timestamp) << "\n";

    file.close();
    return true;
}

qint64 Reporting::getRepoSize(const QString &repoPath) {
    qint64 size = 0;
    QDirIterator it(repoPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        size += it.fileInfo().size();
    }
    return size;
}
