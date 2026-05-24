#include "xml_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace glia::util {

std::shared_ptr<XmlNode> XmlParser::parse(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        file.open("../" + path);
        if (!file.is_open()) return nullptr;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    size_t pos = 0;
    if (content.find("<?xml") == 0) pos = content.find("?>") + 2;

    skipWhitespace(content, pos);
    return parseElement(content, pos);
}

void XmlParser::skipWhitespace(const std::string& xml, size_t& pos) {
    while (pos < xml.length() && std::isspace(xml[pos])) pos++;
    if (pos + 4 <= xml.length() && xml.substr(pos, 4) == "<!--") {
        size_t end = xml.find("-->", pos);
        if (end != std::string::npos) {
            pos = end + 3;
            skipWhitespace(xml, pos);
        }
    }
}

std::shared_ptr<XmlNode> XmlParser::parseElement(const std::string& xml, size_t& pos) {
    if (pos >= xml.length() || xml[pos] != '<') return nullptr;
    pos++;

    auto node = std::make_shared<XmlNode>();
    size_t nameStart = pos;
    while (pos < xml.length() && !std::isspace(xml[pos]) && xml[pos] != '>' && xml[pos] != '/') pos++;
    node->name = xml.substr(nameStart, pos - nameStart);

    while (pos < xml.length() && xml[pos] != '>' && xml[pos] != '/') {
        skipWhitespace(xml, pos);
        if (xml[pos] == '>' || xml[pos] == '/') break;

        size_t attrStart = pos;
        while (pos < xml.length() && xml[pos] != '=') pos++;
        std::string attrName = xml.substr(attrStart, pos - attrStart);
        pos++;

        char quote = xml[pos++];
        size_t valStart = pos;
        while (pos < xml.length() && xml[pos] != quote) pos++;
        node->attributes[attrName] = decodeEntities(xml.substr(valStart, pos - valStart));
        pos++;
        skipWhitespace(xml, pos);
    }

    if (xml[pos] == '/') {
        pos += 2;
        return node;
    }
    pos++;

    while (pos < xml.length()) {
        skipWhitespace(xml, pos);
        if (pos + 2 <= xml.length() && xml.substr(pos, 2) == "</") {
            pos = xml.find(">", pos) + 1;
            break;
        }
        if (xml[pos] == '<') {
            auto child = parseElement(xml, pos);
            if (child) node->children.push_back(child);
        } else {
            size_t contStart = pos;
            while (pos < xml.length() && xml[pos] != '<') pos++;
            node->content += decodeEntities(xml.substr(contStart, pos - contStart));
        }
    }
    return node;
}

std::string XmlParser::decodeEntities(std::string s) {
    size_t pos = 0;
    while ((pos = s.find("&amp;", pos)) != std::string::npos) { s.replace(pos, 5, "&"); pos += 1; }
    pos = 0;
    while ((pos = s.find("&lt;", pos)) != std::string::npos) { s.replace(pos, 4, "<"); pos += 1; }
    pos = 0;
    while ((pos = s.find("&gt;", pos)) != std::string::npos) { s.replace(pos, 4, ">"); pos += 1; }
    return s;
}

}
