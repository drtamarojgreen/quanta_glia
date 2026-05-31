#include "command_loader.h"
#include "../util/xml_parser.h"
#include "../util/benchmark_utils.h"
#include <sstream>
#include <iostream>
#include <filesystem>

namespace glia::app {

std::vector<CommandMetadata> CommandLoader::loadFromXml(const std::string& path) {
    using glia::util::BenchmarkTimer;
    BenchmarkTimer::start("xml_parse");
    std::vector<CommandMetadata> commands;

    // Load from main rules file
    auto root = glia::util::XmlParser::parse(path);
    BenchmarkTimer::stop("xml_parse");
    if (root) {
        for (const auto& child : root->children) {
            if (child->name == "command") {
                CommandMetadata meta;
                meta.name = child->getAttribute("name");
                meta.description = child->getAttribute("description");
                meta.type = child->getAttribute("type");
                meta.target = child->getAttribute("target");

                for (const auto& sub : child->children) {
                    if (sub->name == "param") {
                        meta.params[sub->getAttribute("key")] = sub->getAttribute("value");
                    } else if (sub->name == "list") {
                        std::string listName = sub->getAttribute("name");
                        for (const auto& item : sub->children) {
                            if (item->name == "item") {
                                meta.lists[listName].push_back(item->content);
                            }
                        }
                    }
                }
                commands.push_back(meta);
            }
        }
    }

    // Load from plugins directory
    namespace fs = std::filesystem;
    if (fs::exists("plugins")) {
        for (const auto& entry : fs::directory_iterator("plugins")) {
            if (entry.path().extension() == ".xml") {
                auto pRoot = glia::util::XmlParser::parse(entry.path().string());
                if (!pRoot) continue;
                for (const auto& child : pRoot->children) {
                    if (child->name == "command") {
                        CommandMetadata meta;
                        meta.name = child->getAttribute("name");
                        meta.description = child->getAttribute("description");
                        meta.type = child->getAttribute("type");
                        meta.target = child->getAttribute("target");
                        commands.push_back(meta);
                    }
                }
            }
        }
    }

    return commands;
}

RuleGlobals CommandLoader::loadGlobals(const std::string& path) {
    RuleGlobals g;
    auto root = glia::util::XmlParser::parse(path);
    if (!root) return g;

    for (const auto& child : root->children) {
        if (child->name == "globals") {
            for (const auto& sub : child->children) {
                if (sub->name == "extensions") {
                    std::stringstream ss(sub->content);
                    std::string item;
                    while (std::getline(ss, item, ',')) g.extensions.push_back(item);
                } else if (sub->name == "ignored_dirs") {
                    std::stringstream ss(sub->content);
                    std::string item;
                    while (std::getline(ss, item, ',')) g.ignoredDirs.push_back(item);
                } else if (sub->name == "ui_strings") {
                    for (const auto& strNode : sub->children) {
                        if (strNode->name == "string") {
                            g.uiStrings[strNode->getAttribute("key")] = strNode->getAttribute("value");
                        }
                    }
                }
            }
        }
    }
    return g;
}

}
