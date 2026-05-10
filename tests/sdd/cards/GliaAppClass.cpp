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

// @Card: glia_app_config_serialization
// @Results glia_app_config_xml_operational == true, glia_app_config_json_operational == true
void glia_app_config_serialization_card(const std::map<std::string, std::string>& facts) {
    GliaConfig config;
    config.setKnowledgeBase("/tmp/kb");
    config.setMaxRepos(42);

    QString xmlPath = "/tmp/test_config.xml";
    QString jsonPath = "/tmp/test_config.json";

    bool xml_saved = GliaConfigManager::saveToXml(config, xmlPath);
    GliaConfig xmlLoad;
    bool xml_loaded = GliaConfigManager::loadFromXml(xmlLoad, xmlPath);
    bool xml_op = xml_saved && xml_loaded && (xmlLoad.maxRepos() == 42) && (xmlLoad.knowledgeBase() == "/tmp/kb");

    bool json_saved = GliaConfigManager::saveToJson(config, jsonPath);
    GliaConfig jsonLoad;
    bool json_loaded = GliaConfigManager::loadFromJson(jsonLoad, jsonPath);
    bool json_op = json_saved && json_loaded && (jsonLoad.maxRepos() == 42) && (jsonLoad.knowledgeBase() == "/tmp/kb");

    std::cout << "glia_app_config_xml_operational = " << (xml_op ? "true" : "false") << std::endl;
    std::cout << "glia_app_config_json_operational = " << (json_op ? "true" : "false") << std::endl;
}

// @Card: glia_app_qprocess_integration
// @Results glia_app_qprocess_operational == true
void glia_app_qprocess_integration_card(const std::map<std::string, std::string>& facts) {
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

    bool qprocess_op = started && finished && (output == "success");
    std::cout << "glia_app_qprocess_operational = " << (qprocess_op ? "true" : "false") << std::endl;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    auto facts = Sorrel::Sdd::Util::FactReader::readFacts("tests/sdd/facts/environment.facts");

    if (argc > 1) {
        std::string card = argv[1];
        if (card == "glia_app_config_serialization") {
            glia_app_config_serialization_card(facts);
        } else if (card == "glia_app_qprocess_integration") {
            glia_app_qprocess_integration_card(facts);
        }
    } else {
        glia_app_config_serialization_card(facts);
        glia_app_qprocess_integration_card(facts);
    }

    return 0;
}
