#ifndef GLIA_APP_GLIA_DOC_CMDS_H
#define GLIA_APP_GLIA_DOC_CMDS_H

#include "command.h"

namespace glia::app {

class DocSpellcheckCommand : public Command {
public:
    std::string name() const override { return "doc-spellcheck"; }
    std::string description() const override { return "Spellcheck Markdown files"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class DocTodoListCommand : public Command {
public:
    std::string name() const override { return "doc-todo-list"; }
    std::string description() const override { return "Extract TODOs from codebase"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class DocCreateReadmeCommand : public Command {
public:
    std::string name() const override { return "doc-create-readme"; }
    std::string description() const override { return "Generate project README.md"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

class DocUpdateChangelogCommand : public Command {
public:
    std::string name() const override { return "doc-update-changelog"; }
    std::string description() const override { return "Update CHANGELOG.md from git history"; }
    glia::core::CommandResult execute(const std::vector<std::string>& args) override;
};

}

#endif
