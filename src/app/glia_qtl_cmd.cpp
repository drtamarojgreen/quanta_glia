#include "glia_qtl_cmd.h"
#include "../util/xml_parser.h"
#include "../util/fs_utils.h"
#include "../util/string_utils.h"
#include "../cli/cli.h"
#include "command_loader.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

namespace glia::app {

glia::core::CommandResult QtlCommand::execute(const std::vector<std::string>& args) {
    std::string qtlPath = glia::util::findRepoRoot() + "/data/qtl.xml";
    auto tasks = loadQtlTasks(qtlPath);

    analyzeRepository(tasks);

    std::string output = formatTaskList(tasks);

    std::string filename;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].find("--filename=") == 0) {
            filename = args[i].substr(11);
            break;
        }
        if (args[i] == "--filename" && i + 1 < args.size()) {
            filename = args[i+1];
            break;
        }
    }

    if (!filename.empty()) {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << output;
            outFile.close();
            return {glia::core::ExitCode::Success, "Task list saved to " + filename};
        } else {
            return {glia::core::ExitCode::InternalFailure, "Failed to open file for writing: " + filename};
        }
    }

    std::cout << output << std::endl;
    return {glia::core::ExitCode::Success, "Analysis complete"};
}

std::vector<QtlTask> QtlCommand::loadQtlTasks(const std::string& path) {
    std::vector<QtlTask> tasks;
    auto root = glia::util::XmlParser::parse(path);
    if (!root) return tasks;

    for (const auto& child : root->children) {
        if (child->name == "qtl:Task") {
            QtlTask task;
            task.id = child->getAttribute("id");
            for (const auto& sub : child->children) {
                if (sub->name == "qtl:Title") task.title = sub->content;
                else if (sub->name == "qtl:ErrorDescription") task.description = sub->content;
                else if (sub->name == "qtl:Category") task.category = sub->content;
                else if (sub->name == "qtl:Priority") task.priority = sub->content;
                else if (sub->name == "qtl:Symptoms") {
                    for (const auto& sym : sub->children) {
                        if (sym->name == "qtl:Symptom") task.symptoms.push_back(sym->content);
                    }
                }
                else if (sub->name == "qtl:RemediationTasks") {
                    for (const auto& rem : sub->children) {
                        if (rem->name == "qtl:RemediationTask") task.remediationTasks.push_back(rem->content);
                    }
                }
            }
            tasks.push_back(task);
        }
    }
    return tasks;
}

void QtlCommand::analyzeRepository(std::vector<QtlTask>& tasks) {
    std::string rulesPath = glia::util::findRulesXml();
    auto globals = CommandLoader::loadGlobals(rulesPath);
    std::string root = glia::util::findRepoRoot();

    // Define symptom to regex mapping (mapping task symptom strings to detection patterns)
    std::map<std::string, std::regex> symptomPatterns = {
        {"TODO comments", std::regex("TODO|FIXME")},
        {"Empty catch blocks", std::regex("catch\\s*\\([^\\)]*\\)\\s*\\{\\s*\\}")},
        {"assert(true)", std::regex("assert\\s*\\(\\s*(true|1|!false|!!true)\\s*\\)")},
        {"return true;", std::regex("return\\s+true\\s*;")},
        {"return nullptr;", std::regex("return\\s+nullptr\\s*;")},
        {"Embedded file paths", std::regex("\"[a-zA-Z0-9_/]+\\.[a-zA-Z0-9]+\"")},
        {"Assuming Linux-only commands", std::regex("\\b(ls|cd|mkdir|rm|cp|mv|grep|cat|chmod|chown)\\b")},
        {"Raw Pointer Management", std::regex("\\b(new\\s+[^\\(;]+|delete\\s+[^;]+)\\b")},
        {"TODO|FIXME", std::regex("TODO|FIXME")},
        {"Empty Catch Block", std::regex("catch\\s*\\([^\\)]*\\)\\s*\\{\\s*\\}")},
        {"Meaningless Assertion", std::regex("assert\\s*\\(\\s*(true|1|!false|!!true)\\s*\\)")},
        {"boilerplate|skeletal|placeholder|stub", std::regex("boilerplate|skeletal|placeholder|stub", std::regex::icase)},
        {"as an AI language model|certainly, I can help", std::regex("as an AI language model|certainly, I can help", std::regex::icase)}
    };

    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().filename() == "qtl_output.txt") continue;

        bool ignored = false;
        for (const auto& idir : globals.ignoredDirs) {
            if (entry.path().string().find("/" + idir + "/") != std::string::npos ||
                entry.path().string().find("./" + idir + "/") == 0) {
                ignored = true;
                break;
            }
        }
        if (ignored) continue;

        std::string ext = entry.path().extension().string();
        bool validExt = false;
        for (const auto& e : globals.extensions) if (e == ext) { validExt = true; break; }
        if (!validExt) continue;

        std::ifstream file(entry.path());
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        for (auto& task : tasks) {
            for (const auto& symptom : task.symptoms) {
                if (symptomPatterns.count(symptom)) {
                    auto found = glia::util::findViolations(symptom, content, symptomPatterns[symptom]);
                    if (!found.empty()) {
                        std::cout << "DEBUG: Found " << found.size() << " violations for symptom: " << symptom << " in " << entry.path().filename() << std::endl;
                        task.violationCount += static_cast<int>(found.size());
                    }
                } else {
                    // Fallback for symptoms not in mapping - simple regex search
                    try {
                        std::regex fallbackRegex(symptom, std::regex::icase);
                        if (std::regex_search(content, fallbackRegex)) {
                             task.violationCount++;
                        }
                    } catch (...) {}
                }
            }
        }
    }
}

std::string QtlCommand::formatTaskList(const std::vector<QtlTask>& tasks) {
    std::stringstream ss;
    ss << "==========================================================\n";
    ss << "           GLIA QUEUED TASK LIST (QTL) REPORT             \n";
    ss << "==========================================================\n\n";

    for (const auto& task : tasks) {
        if (task.violationCount > 0) {
            ss << "[" << task.id << "] " << task.title << " (" << task.priority << ")\n";
            ss << "Category: " << task.category << "\n";
            ss << "Status: " << task.violationCount << " potential violations detected\n";
            ss << "Description: " << task.description << "\n";
            ss << "Remediation Tasks:\n";
            for (const auto& rem : task.remediationTasks) {
                ss << "  [ ] " << rem << "\n";
            }
            ss << "\n----------------------------------------------------------\n\n";
        }
    }

    bool anyViolations = false;
    for (const auto& task : tasks) if (task.violationCount > 0) anyViolations = true;

    if (!anyViolations) {
        ss << "No major QTL violations detected in the current repository state.\n";
    }

    return ss.str();
}

}
