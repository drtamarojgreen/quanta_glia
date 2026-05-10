#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include "glia_config.h"
#include "glia_config_manager.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("SORREL-SDD Standalone");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("SORREL Standalone Glia Management Application (SDD)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configOption(QStringList() << "c" << "config", "Path to configuration file.", "file");
    parser.addOption(configOption);

    QCommandLineOption actionOption(QStringList() << "a" << "action", "Action to perform (harvest, prune).", "action");
    parser.addOption(actionOption);

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

    if (action == "harvest") {
        if (repos.isEmpty()) {
            qCritical() << "No repositories provided for harvest.";
            return 1;
        }

        QProcess process;
        QStringList args;
        args << "scripts/quanta_glia.py";
        if (!configPath.isEmpty()) {
            args << "--config" << configPath;
        }
        args << repos;

        qInfo() << "Starting SORREL Harvester...";
        process.start("python3", args);
        if (!process.waitForStarted()) {
            qCritical() << "Failed to start python3 scripts/quanta_glia.py";
            return 1;
        }

        process.waitForFinished();
        qInfo() << "Harvester finished with exit code" << process.exitCode();
        qInfo() << "Output:" << process.readAllStandardOutput();

    } else if (action == "prune") {
        QProcess process;
        QStringList args;
        args << "scripts/pruner.py";

        qInfo() << "Starting SORREL Pruner...";
        process.start("python3", args);
        if (!process.waitForStarted()) {
            qCritical() << "Failed to start python3 scripts/pruner.py";
            return 1;
        }

        process.waitForFinished();
        qInfo() << "Pruner finished with exit code" << process.exitCode();
        qInfo() << "Output:" << process.readAllStandardOutput();
    } else {
        qInfo() << "No action specified or unknown action. Use --help for usage.";
    }

    return 0;
}
