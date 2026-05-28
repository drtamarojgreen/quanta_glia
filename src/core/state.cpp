#include "state.h"
#include <fstream>
#include <iostream>
#include <regex>

namespace glia::core {
std::map<std::string, std::string>& State::data() {
    static std::map<std::string, std::string> instance;
    return instance;
}

void State::set(const std::string& key, const std::string& val) {
    data()[key] = val;
}

std::string State::get(const std::string& key, const std::string& defaultVal) {
    if (data().count(key)) return data().at(key);
    return defaultVal;
}

void State::save(const std::string& path) {
    std::ofstream out(path);
    out << "{\n";
    for (auto it = data().begin(); it != data().end(); ++it) {
        out << "  \"" << it->first << "\": \"" << it->second << "\"";
        if (std::next(it) != data().end()) out << ",";
        out << "\n";
    }
    out << "}\n";
}

void State::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::regex pairRegex("\"([^\"]+)\":\\s*\"([^\"]+)\"");
    auto pair_begin = std::sregex_iterator(content.begin(), content.end(), pairRegex);
    auto pair_end = std::sregex_iterator();
    for (std::sregex_iterator i = pair_begin; i != pair_end; ++i) {
        std::smatch match = *i;
        data()[match[1]] = match[2];
    }
}
}
