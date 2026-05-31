#ifndef GLIA_APP_QTL_CMD_H
#define GLIA_APP_QTL_CMD_H

#include "command.h"
#include <string>
#include <vector>
#include <map>
#include <regex>

namespace glia::app {

struct QtlTask {
    std::string id;
    std::string title;
    std::string description;
    std::string category;
    std::string priority;
    std::vector<std::string> symptoms;
    std::vector<std::string> remediationTasks;
    int violationCount = 0;
};

class QtlCommand : public Command {
public:
    std::string name() const override { return "qtl"; }
    std::string description() const override { return "Analyze repository for QTL violations and generate task list"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;

private:
    std::vector<QtlTask> loadQtlTasks(const std::string& path);
    void analyzeRepository(std::vector<QtlTask>& tasks);
    std::string formatTaskList(const std::vector<QtlTask>& tasks);
};

}

#endif
