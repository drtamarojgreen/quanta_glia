#ifndef GLIA_UTIL_XML_PARSER_H
#define GLIA_UTIL_XML_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace glia::util {

struct XmlNode {
    std::string name;
    std::string content;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<XmlNode>> children;

    std::string getAttribute(const std::string& key) const {
        return attributes.count(key) ? attributes.at(key) : "";
    }
};

class XmlParser {
public:
    static std::shared_ptr<XmlNode> parse(const std::string& path);
private:
    static std::shared_ptr<XmlNode> parseElement(const std::string& xml, size_t& pos);
    static void skipWhitespace(const std::string& xml, size_t& pos);
    static std::string decodeEntities(std::string s);
};

}

#endif
