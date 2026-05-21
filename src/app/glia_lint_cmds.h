#ifndef GLIA_APP_GLIA_LINT_CMDS_H
#define GLIA_APP_GLIA_LINT_CMDS_H

#include "command.h"

namespace glia::app {

class LintCheckFormatCommand : public Command {
public:
    std::string name() const override { return "lint-check-format"; }
    std::string description() const override { return "Verify code formatting"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintCommentsCommand : public Command {
public:
    std::string name() const override { return "lint-comments"; }
    std::string description() const override { return "Check for missing/outdated comments"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintNamingCommand : public Command {
public:
    std::string name() const override { return "lint-naming"; }
    std::string description() const override { return "Verify naming conventions"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintSecurityCommand : public Command {
public:
    std::string name() const override { return "lint-security"; }
    std::string description() const override { return "Static security analysis (SAST)"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintSecretsCommand : public Command {
public:
    std::string name() const override { return "lint-secrets"; }
    std::string description() const override { return "Scan for accidentally committed secrets"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintPerfHintsCommand : public Command {
public:
    std::string name() const override { return "lint-perf-hints"; }
    std::string description() const override { return "Performance optimization hints"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintDeprecatedCommand : public Command {
public:
    std::string name() const override { return "lint-deprecated"; }
    std::string description() const override { return "Identify usage of deprecated APIs"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintHeadersCommand : public Command {
public:
    std::string name() const override { return "lint-headers"; }
    std::string description() const override { return "Verify license headers"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintMarkdownCommand : public Command {
public:
    std::string name() const override { return "lint-markdown"; }
    std::string description() const override { return "Lint Markdown files"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintYamlCommand : public Command {
public:
    std::string name() const override { return "lint-yaml"; }
    std::string description() const override { return "Validate YAML files"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintJsonCommand : public Command {
public:
    std::string name() const override { return "lint-json"; }
    std::string description() const override { return "Validate JSON files"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class LintDockerfileCommand : public Command {
public:
    std::string name() const override { return "lint-dockerfile"; }
    std::string description() const override { return "Lint Dockerfiles"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
