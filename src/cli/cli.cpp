#include "cli.h"
#include <iomanip>
#include <iostream>

namespace glia::cli {

void Terminal::color(const std::string& code) { std::cout << "\033[" << code << "m"; }
void Terminal::reset() { std::cout << "\033[0m"; }

void Terminal::printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) {
    std::vector<size_t> widths;
    for (const auto& h : headers) widths.push_back(h.size());
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i < widths.size()) widths[i] = std::max(widths[i], row[i].size());
        }
    }

    for (size_t i = 0; i < headers.size(); ++i) std::cout << std::left << std::setw(widths[i] + 2) << headers[i];
    std::cout << "\n";
    for (auto w : widths) std::cout << std::string(w + 1, '-') << " ";
    std::cout << "\n";
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) std::cout << std::left << std::setw(widths[i] + 2) << row[i];
        std::cout << "\n";
    }
}

void ArgumentParser::addOption(const std::string& name, const std::string& alias, const std::string& description) {
    aliases[alias] = name;
    descriptions[name] = description;
}

bool ArgumentParser::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--") {
            for (int j = i + 1; j < argc; ++j) positional.push_back(argv[j]);
            break;
        }
        if (arg.size() > 1 && arg[0] == '-') {
            std::string name = arg;
            if (aliases.count(name)) name = aliases[name];
            if (i + 1 < argc && argv[i+1][0] != '-') options[name] = argv[++i];
            else flags[name] = true;
        } else positional.push_back(arg);
    }
    return true;
}

std::string ArgumentParser::getOption(const std::string& name) const { return options.count(name) ? options.at(name) : ""; }
bool ArgumentParser::hasFlag(const std::string& name) const { return flags.count(name); }
std::vector<std::string> ArgumentParser::getPositional() const { return positional; }

void ArgumentParser::printHelp() const {
    std::cout << "Options:\n";
    for (const auto& [name, desc] : descriptions) {
        std::cout << "  " << std::left << std::setw(20) << name << desc << "\n";
    }
}

bool Prompter::confirm(const std::string& message) {
    std::cout << message << " [y/N]: ";
    std::string line; std::getline(std::cin, line);
    return !line.empty() && (line[0] == 'y' || line[0] == 'Y');
}

std::string Prompter::ask(const std::string& message) {
    std::cout << message << ": ";
    std::string line; std::getline(std::cin, line);
    return line;
}

}
