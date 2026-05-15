#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "util/fact_utils.h"
#include "glia_config.h"
#include "glia_config_manager.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDir>

// @Card: sorrel_glia_config_serialization
// @Results sorrel_glia_config_xml_operational == true, sorrel_glia_config_json_operational == true
void sorrel_glia_config_serialization_card(const std::map<std::string, std::string>& facts) {
    GliaConfig config;
    config.setKnowledgeBase("/tmp/kb");
    config.setMaxRepos(42);

    QString xmlPath = "/tmp/test_config.xml";
    QString jsonPath = "/tmp/test_config.json";

    bool xml_saved = GliaConfigManager::saveToXml(config, xmlPath);
    GliaConfig xmlLoad;
    bool xml_loaded = GliaConfigManager::loadFromXml(xmlLoad, xmlPath);
    int xml_op = (xml_saved && xml_loaded && (xmlLoad.maxRepos() == 42) && (xmlLoad.knowledgeBase() == "/tmp/kb")) ? 1 : 0;

    bool json_saved = GliaConfigManager::saveToJson(config, jsonPath);
    GliaConfig jsonLoad;
    bool json_loaded = GliaConfigManager::loadFromJson(jsonLoad, jsonPath);
    int json_op = (json_saved && json_loaded && (jsonLoad.maxRepos() == 42) && (jsonLoad.knowledgeBase() == "/tmp/kb")) ? 1 : 0;

    std::cout << "sorrel_glia_config_xml_operational = " << xml_op << std::endl;
    std::cout << "sorrel_glia_config_json_operational = " << json_op << std::endl;
}

// @Card: sorrel_sdd_qprocess_integration
// @Results sorrel_sdd_qprocess_operational == true
void sorrel_sdd_qprocess_integration_card(const std::map<std::string, std::string>& facts) {
    // Create a dummy python script to test QProcess
    QString scriptPath = "/tmp/dummy_script.py";
    QFile file(scriptPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "print('success')\n";
        file.close();
    }

    QProcess process;
    process.start("python3", QStringList() << scriptPath);
    bool started = process.waitForStarted();
    bool finished = process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();

    int qprocess_op = (started && finished && (output == "success")) ? 1 : 0;
    std::cout << "sorrel_sdd_qprocess_operational = " << qprocess_op << std::endl;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    auto facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/environment.facts");

    if (argc > 1) {
        std::string card = argv[1];
        if (card == "sorrel_glia_config_serialization") {
            sorrel_glia_config_serialization_card(facts);
        } else if (card == "sorrel_sdd_qprocess_integration") {
            sorrel_sdd_qprocess_integration_card(facts);
        }
    } else {
        sorrel_glia_config_serialization_card(facts);
        sorrel_sdd_qprocess_integration_card(facts);
    }

    return 0;
}
