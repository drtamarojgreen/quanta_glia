#include "translator.h"

namespace glia::util {

std::map<std::string, std::string>& Translator::strings() {
    static std::map<std::string, std::string> instance;
    return instance;
}

void Translator::load(const std::map<std::string, std::string>& s) {
    strings() = s;
}

std::string Translator::t(const std::string& key) {
    if (strings().count(key)) return strings().at(key);
    return key;
}

}
