#ifndef SDD_REPORTING_H
#define SDD_REPORTING_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDateTime>

struct RepoReportData {
    QString repoName;
    qint64 repoSize;
    int numFiles;
    int numSrcFiles;
    int numDocFiles;
    int numTestFiles;
    QString techStack;
    QString timestamp;
};

class Reporting {
public:
    static RepoReportData analyzeRepository(const QString &repoPath);
    static bool writeToCsv(const RepoReportData &data, const QString &filename = "repository_data.csv");

private:
    static qint64 getRepoSize(const QString &repoPath);
};

#endif // SDD_REPORTING_H
