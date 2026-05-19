#include "EnhancementsClass.h"
#include <iostream>
#include <map>
#include <string>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "glia/modules/reporting/reporting.h"
#include "glia/modules/harvester/harvester.h"
#include "glia/modules/pruner/pruner.h"
#include "glia/core/glia_config.h"
#include "util/fact_utils.h"

using namespace Sorrel::Sdd::Util;

void reporting_enhancement_verification(const std::map<std::string, std::string>& facts) {
    QString repoPath = QDir::tempPath() + "/sdd_report_test_empirical";
    QDir().mkpath(repoPath + "/src");
    QFile f(repoPath + "/src/main.py");
    f.open(QIODevice::WriteOnly); f.write("print('test')"); f.close();

    auto data = Reporting::analyzeRepository(repoPath);

    std::cout << "report_src_files = " << data.numSrcFiles << std::endl;
    std::cout << "report_tech_stack = " << data.techStack.toStdString() << std::endl;
    std::cout << "report_timestamp_format = " << (data.timestamp.endsWith("Z") ? 1 : 0) << std::endl;

    QDir(repoPath).removeRecursively();
}

void harvester_enhancement_verification(const std::map<std::string, std::string>& facts) {
    QString repoPath = QDir::tempPath() + "/sdd_harvest_test_empirical";
    QDir().mkpath(repoPath + "/allowed");
    QDir().mkpath(repoPath + "/forbidden");

    QFile f1(repoPath + "/allowed/README.md");
    f1.open(QIODevice::WriteOnly); f1.write("line 1\nline 2\nline 3\n"); f1.close();

    QFile f2(repoPath + "/forbidden/README.md");
    f2.open(QIODevice::WriteOnly); f2.write("secret"); f2.close();

    GliaConfig config;
    config.setSearchPaths(QStringList() << "/allowed");
    config.setMaxDepth(1);
    config.setMaxLinesPerFile(1);
    config.setTargetTopics(QStringList() << "README");

    auto extracted = Harvester::extractText(repoPath, config);

    std::cout << "harvester_files_extracted = " << extracted.size() << std::endl;
    if (extracted.contains("allowed/README.md")) {
        std::cout << "harvester_line_count = " << extracted["allowed/README.md"].trimmed().split('\n').size() << std::endl;
    } else {
        std::cout << "harvester_line_count = 0" << std::endl;
    }
    std::cout << "harvester_forbidden_found = " << (extracted.contains("forbidden/README.md") ? 1 : 0) << std::endl;

    QDir(repoPath).removeRecursively();
}

void harvester_collision_verification(const std::map<std::string, std::string>& facts) {
    QString repoPath = QDir::tempPath() + "/sdd_harvest_collision_empirical";
    QDir().mkpath(repoPath + "/a");
    QDir().mkpath(repoPath + "/b");

    QFile f1(repoPath + "/a/README.md");
    f1.open(QIODevice::WriteOnly); f1.write("content a"); f1.close();

    QFile f2(repoPath + "/b/README.md");
    f2.open(QIODevice::WriteOnly); f2.write("content b"); f2.close();

    GliaConfig config;
    config.setTargetTopics(QStringList() << "README");

    auto extracted = Harvester::extractText(repoPath, config);

    std::cout << "harvester_unique_paths = " << extracted.size() << std::endl;
    std::cout << "harvester_path_a_exists = " << (extracted.contains("a/README.md") ? 1 : 0) << std::endl;
    std::cout << "harvester_path_b_exists = " << (extracted.contains("b/README.md") ? 1 : 0) << std::endl;

    QDir(repoPath).removeRecursively();
}

void pruner_enhancement_verification(const std::map<std::string, std::string>& facts) {
    QString kbPath = QDir::tempPath() + "/sdd_kb_test_empirical";
    QDir().mkpath(kbPath + "/repo_main");
    QDir().mkpath(kbPath + "/repo_dep");

    QFile f(kbPath + "/repo_main/requirements.txt");
    f.open(QIODevice::WriteOnly); f.write("repo_dep\n"); f.close();

    QSet<QString> deps = Pruner::buildDependencyMap(kbPath);

    std::cout << "pruner_deps_count = " << deps.size() << std::endl;
    std::cout << "pruner_is_dep_protected = " << (deps.contains("repo_dep") ? 1 : 0) << std::endl;

    QDir(kbPath).removeRecursively();
}
