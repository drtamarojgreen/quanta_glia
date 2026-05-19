#ifndef SDD_CONFIG_H
#define SDD_CONFIG_H

#include <QString>
#include <QStringList>

class GliaConfig {
public:
    GliaConfig();

    QString knowledgeBase() const;
    void setKnowledgeBase(const QString &path);

    QString repoCache() const;
    void setRepoCache(const QString &path);

    int maxRepos() const;
    void setMaxRepos(int count);

    QStringList targetTopics() const;
    void setTargetTopics(const QStringList &topics);

    QString logLevel() const;
    void setLogLevel(const QString &level);

    QStringList searchPaths() const;
    void setSearchPaths(const QStringList &paths);

    int maxDepth() const;
    void setMaxDepth(int depth);

    int maxLinesPerFile() const;
    void setMaxLinesPerFile(int lines);

private:
    QString m_knowledgeBase;
    QString m_repoCache;
    int m_maxRepos;
    QStringList m_targetTopics;
    QString m_logLevel;
    QStringList m_searchPaths;
    int m_maxDepth;
    int m_maxLinesPerFile;
};

#endif // SDD_CONFIG_H
