#ifndef GLIA_APP_RULE_LOADER_H
#define GLIA_APP_RULE_LOADER_H

#include "command.h"
#include "shell_command.h"
#include "complex_commands.h"
#include "sequence_command.h"
#include <string>
#include <vector>
#include <fstream>
#include <regex>

namespace glia::app {

class RuleLoader {
public:
    static void load(const std::string& xmlPath, CommandRegistry& registry) {
        std::ifstream file(xmlPath);
        if (!file.is_open()) return;

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        std::regex commandRegex("<Command>([\\s\\S]*?)</Command>");
        std::regex nameRegex("<Name>(.*?)</Name>");
        std::regex descRegex("<Description>(.*?)</Description>");
        std::regex templateRegex("<ShellTemplate>(.*?)</ShellTemplate>");
        std::regex stepsRegex("<Steps>([\\s\\S]*?)</Steps>");
        std::regex stepRegex("<Step>(.*?)</Step>");
        std::regex typeRegex("<Type>(.*?)</Type>");

        auto commandsBegin = std::sregex_iterator(content.begin(), content.end(), commandRegex);
        auto commandsEnd = std::sregex_iterator();

        for (std::sregex_iterator i = commandsBegin; i != commandsEnd; ++i) {
            std::string cmdBlock = (*i).str();
            
            std::smatch match;
            std::string name, desc, shellTemplate, type;
            std::vector<std::string> steps;

            if (std::regex_search(cmdBlock, match, nameRegex)) name = match[1];
            if (std::regex_search(cmdBlock, match, descRegex)) desc = match[1];
            if (std::regex_search(cmdBlock, match, typeRegex)) type = match[1];

            if (name.empty()) continue;

            // Check for multi-step first
            if (std::regex_search(cmdBlock, match, stepsRegex)) {
                std::string stepsBlock = match[1];
                auto stepBegin = std::sregex_iterator(stepsBlock.begin(), stepsBlock.end(), stepRegex);
                auto stepEnd = std::sregex_iterator();
                for (std::sregex_iterator j = stepBegin; j != stepEnd; ++j) {
                    steps.push_back((*j)[1]);
                }
            } else if (std::regex_search(cmdBlock, match, templateRegex)) {
                shellTemplate = match[1];
            }

            if (type == "complex") {
                registerComplex(name, shellTemplate, registry);
            } else if (!steps.empty()) {
                registry.registerCommand(std::make_unique<SequenceCommand>(name, desc, steps));
            } else if (!shellTemplate.empty()) {
                registry.registerCommand(std::make_unique<GenericShellCommand>(name, desc, shellTemplate));
            }
        }
    }

private:
    static void registerComplex(const std::string& name, const std::string& target, CommandRegistry& registry) {
        if (target == "#complex:git-cleanup-local") {
            registry.registerCommand(std::make_unique<GitCleanupLocalCommand>());
        } else if (target == "#complex:doc-todo-list") {
            registry.registerCommand(std::make_unique<DocTodoListCommand>());
        } else if (target == "#complex:proj-estimate") {
            registry.registerCommand(std::make_unique<ProjEstimateCommand>());
        } else if (target == "#complex:sys-alias-setup") {
            registry.registerCommand(std::make_unique<SysAliasSetupCommand>());
        }
    }
};

}

#endif
