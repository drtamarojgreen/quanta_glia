#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include "glia_config.h"
#include "glia_config_manager.h"
#include "harvester.h"
#include "pruner.h"
#include "reporting.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("QuantaGlia Standalone");
    QCoreApplication::setApplicationVersion("1.1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("QuantaGlia Standalone Management Application (SDD Phase III)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configOption(QStringList() << "c" << "config", "Path to configuration file.", "file");
    parser.addOption(configOption);

    QCommandLineOption actionOption(QStringList() << "a" << "action", "Action to perform (harvest, prune, report).", "action");
    parser.addOption(actionOption);

    QCommandLineOption dryRunOption("dry-run", "Perform a dry run.");
    parser.addOption(dryRunOption);

    QCommandLineOption strategyOption("strategy", "Pruning strategy (conservative, aggressive).", "strategy", "conservative");
    parser.addOption(strategyOption);

    parser.addPositionalArgument("repos", "Repository URLs/Paths for harvesting.");

    parser.process(app);

    QString configPath = parser.value(configOption);
    GliaConfig config;
    if (!configPath.isEmpty() && QFileInfo::exists(configPath)) {
        if (configPath.endsWith(".xml")) {
            GliaConfigManager::loadFromXml(config, configPath);
        } else if (configPath.endsWith(".json")) {
            GliaConfigManager::loadFromJson(config, configPath);
        }
    }

    QString action = parser.value(actionOption);
    QStringList repos = parser.positionalArguments();
    bool dryRun = parser.isSet(dryRunOption);

    if (action == "harvest") {
        if (repos.isEmpty()) {
            qCritical() << "No repositories provided for harvest.";
            return 1;
        }
        QDir().mkpath(config.repoCache());
        for (const QString &url : repos) {
            qInfo() << "--- Processing Repository:" << url << "---";
            QString path = Harvester::cloneRepo(url, config.repoCache());
            if (!path.isEmpty()) {
                auto extracted = Harvester::extractText(path, config);
                if (extracted.isEmpty()) {
                    qWarning() << "No target files found in" << url;
                } else {
                    if (Harvester::persistToKnowledgeBase(QFileInfo(path).fileName(), extracted, config.knowledgeBase())) {
                        qInfo() << "Successfully persisted" << extracted.size() << "files to knowledge base.";
                    } else {
                        qCritical() << "Failed to persist files for" << url;
                    }
                }
            }
        }
    } else if (action == "prune") {
        qInfo() << "Starting QuantaGlia Pruner (C++)...";
        QSet<QString> dependencies = Pruner::buildDependencyMap(config.knowledgeBase());
        qInfo() << "Active dependencies found:" << dependencies.values();

        QString strategy = parser.value(strategyOption);

        QDir kb(config.knowledgeBase());
        QStringList reposInKb = kb.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        QMap<QString, float> weights = {{"usage", 0.5f}, {"age", 0.2f}, {"redundancy", 0.2f}, {"ethics_risk", 0.1f}};
        QMap<QString, float> thresholds = {{"delete", 0.9f}, {"archive", 0.7f}, {"review", 0.5f}};

        for (const QString &repoName : reposInKb) {
            if (dependencies.contains(repoName)) {
                qInfo() << "SKIPPING pruning of" << repoName << "(Active Dependency)";
                continue;
            }
            QString path = kb.absoluteFilePath(repoName);
            RepoPruneRecord record;
            record.name = repoName;
            record.path = path;
            record.lastActivity = QFileInfo(path).lastModified();
            record.usageScore = 0.0f; // Mapped from Phase II mock
            record.redundancyScore = 0.0f;
            record.ethicsRiskScore = 0.0f;

            float score = Pruner::calculateCompositeScore(record, weights);
            QString decision = Pruner::makePruningDecision(score, thresholds);

            if (decision == "DELETE" && strategy != "aggressive") {
                qInfo() << "SKIPPING DELETE of" << repoName << "(Strategy is" << strategy << ")";
                continue;
            }

            qInfo() << "Evaluated" << repoName << ": Score=" << score << "Decision=" << decision;
            Pruner::performPruning(path, decision, "repo_archive/", dryRun);
        }
    } else if (action == "report") {
        qInfo() << "Generating QuantaGlia Report (C++)...";
        QDir kb(config.knowledgeBase());
        QStringList reposInKb = kb.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &repoName : reposInKb) {
            QString path = kb.absoluteFilePath(repoName);
            auto data = Reporting::analyzeRepository(path);
            if (Reporting::writeToCsv(data)) {
                qInfo() << "Reported" << repoName << "(Stack:" << data.techStack << ")";
            }
        }
    } else {
        qInfo() << "No action specified. Use --help for usage.";
    }
    return 0;
}
