#include "glia_config.h"

GliaConfig::GliaConfig()
    : m_knowledgeBase("./knowledge_base")
    , m_repoCache("./repo_cache")
    , m_maxRepos(10)
{
    m_targetTopics << "README" << "LICENSE" << "CONTRIBUTING";
}

QString GliaConfig::knowledgeBase() const { return m_knowledgeBase; }
void GliaConfig::setKnowledgeBase(const QString &path) { m_knowledgeBase = path; }

QString GliaConfig::repoCache() const { return m_repoCache; }
void GliaConfig::setRepoCache(const QString &path) { m_repoCache = path; }

int GliaConfig::maxRepos() const { return m_maxRepos; }
void GliaConfig::setMaxRepos(int count) { m_maxRepos = count; }

QStringList GliaConfig::targetTopics() const { return m_targetTopics; }
void GliaConfig::setTargetTopics(const QStringList &topics) { m_targetTopics = topics; }
