#ifndef GLIA_CONFIG_H
#define GLIA_CONFIG_H

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

private:
    QString m_knowledgeBase;
    QString m_repoCache;
    int m_maxRepos;
    QStringList m_targetTopics;
};

#endif // GLIA_CONFIG_H
