#ifndef GLIA_UTIL_TRANSLATOR_H
#define GLIA_UTIL_TRANSLATOR_H

#include <string>
#include <map>

namespace glia::util {

class Translator {
public:
    static void load(const std::map<std::string, std::string>& strings);
    static std::string t(const std::string& key);
private:
    static std::map<std::string, std::string>& strings();
};

}

#endif
