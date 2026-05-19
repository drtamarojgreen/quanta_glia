#include "glia_config.h"

GliaConfig::GliaConfig()
    : m_knowledgeBase("./knowledge_base")
    , m_repoCache("./repo_cache")
    , m_maxRepos(10)
    , m_logLevel("INFO")
    , m_maxDepth(5)
    , m_maxLinesPerFile(500)
{
    m_targetTopics << "README" << "LICENSE" << "CONTRIBUTING";
    m_searchPaths << "/";
}

QString GliaConfig::knowledgeBase() const { return m_knowledgeBase; }
void GliaConfig::setKnowledgeBase(const QString &path) { m_knowledgeBase = path; }

QString GliaConfig::repoCache() const { return m_repoCache; }
void GliaConfig::setRepoCache(const QString &path) { m_repoCache = path; }

int GliaConfig::maxRepos() const { return m_maxRepos; }
void GliaConfig::setMaxRepos(int count) { m_maxRepos = count; }

QStringList GliaConfig::targetTopics() const { return m_targetTopics; }
void GliaConfig::setTargetTopics(const QStringList &topics) { m_targetTopics = topics; }

QString GliaConfig::logLevel() const { return m_logLevel; }
void GliaConfig::setLogLevel(const QString &level) { m_logLevel = level; }

QStringList GliaConfig::searchPaths() const { return m_searchPaths; }
void GliaConfig::setSearchPaths(const QStringList &paths) { m_searchPaths = paths; }

int GliaConfig::maxDepth() const { return m_maxDepth; }
void GliaConfig::setMaxDepth(int depth) { m_maxDepth = depth; }

int GliaConfig::maxLinesPerFile() const { return m_maxLinesPerFile; }
void GliaConfig::setMaxLinesPerFile(int lines) { m_maxLinesPerFile = lines; }
