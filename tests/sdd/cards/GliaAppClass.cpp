#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "util/fact_utils.h"
#include "glia_config.h"
#include "glia_config_manager.h"
#include "reporting.h"
#include "harvester.h"
#include "pruner.h"
#include "EnhancementsClass.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QTextStream>

void sorrel_glia_config_serialization_card(const std::map<std::string, std::string>& facts) {
    GliaConfig config;
    config.setKnowledgeBase("/tmp/kb");
    config.setMaxRepos(42);
    config.setLogLevel("DEBUG");

    QString xmlPath = QDir::tempPath() + "/test_config_empirical.xml";
    GliaConfigManager::saveToXml(config, xmlPath);
    GliaConfig xmlLoad;
    GliaConfigManager::loadFromXml(xmlLoad, xmlPath);

    std::cout << "config_xml_max_repos = " << xmlLoad.maxRepos() << std::endl;
    std::cout << "config_xml_kb = " << xmlLoad.knowledgeBase().toStdString() << std::endl;
    std::cout << "config_xml_log_level = " << xmlLoad.logLevel().toStdString() << std::endl;
}

void sorrel_sdd_qprocess_integration_card(const std::map<std::string, std::string>& facts) {
    QString scriptPath = QDir::tempPath() + "/dummy_script_empirical.py";
    QFile file(scriptPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "print('empirical_success')\n";
        file.close();
    }

    QProcess process;
    process.start("python3", QStringList() << scriptPath);
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();

    std::cout << "qprocess_output = " << output.toStdString() << std::endl;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    auto facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/environment.facts");
    auto enh_facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/enhancements.facts");
    facts.insert(enh_facts.begin(), enh_facts.end());

    sorrel_glia_config_serialization_card(facts);
    sorrel_sdd_qprocess_integration_card(facts);
    reporting_enhancement_verification(facts);
    harvester_enhancement_verification(facts);
    pruner_enhancement_verification(facts);
    harvester_collision_verification(facts);

    return 0;
}
