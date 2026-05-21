#ifndef GLIA_APP_GLIA_PROJ_CMDS_H
#define GLIA_APP_GLIA_PROJ_CMDS_H

#include "command.h"

namespace glia::app {

class ProfBinaryDiffCommand : public Command {
public:
    std::string name() const override { return "prof-binary-diff"; }
    std::string description() const override { return "Compare binary size and symbols"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class CodeReviewSubmitCommand : public Command {
public:
    std::string name() const override { return "code-review-submit"; }
    std::string description() const override { return "Submit a code review"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class FeedbackCollectCommand : public Command {
public:
    std::string name() const override { return "feedback-collect"; }
    std::string description() const override { return "Run internal tool feedback survey"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class ProjInfoCommand : public Command {
public:
    std::string name() const override { return "proj-info"; }
    std::string description() const override { return "Display project metadata and metrics"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class ProjHealthCommand : public Command {
public:
    std::string name() const override { return "proj-health"; }
    std::string description() const override { return "Calculate project health score"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class ProjTechStackCommand : public Command {
public:
    std::string name() const override { return "proj-tech-stack"; }
    std::string description() const override { return "Identify technologies used in project"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class ProjEstimateCommand : public Command {
public:
    std::string name() const override { return "proj-estimate"; }
    std::string description() const override { return "Estimate project size (LOC, files)"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
