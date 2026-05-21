#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include "produce_resolutions.h"
#include "config.h"

void test_produce_no_value_errors() {
    auto resolutions = Config::loadResolutions("test/test-resolutions.yaml");
    auto patterns = Config::loadExtractionPatterns("test/test-extraction_patterns.yaml");
    ErrorObject obj;
    obj.entries.push_back({"TYPE", "file1.cpp", 10, "error: 'x' TypeError line"});
    obj.entries.push_back({"WARNING", "file2.cpp", 11, "Warning line"});
    obj.entries.push_back({"ERROR", "file3.cpp", 12, "Other error line"});

    ProduceResolutions resolver(resolutions, patterns);
    auto alts = resolver.produce(obj);

    // TYPE (4) + WARNING (3) + ERROR (3) = 10
    assert(alts.size() == 10);
}

void test_produce_single_value_error() {
    auto resolutions = Config::loadResolutions("test/test-resolutions.yaml");
    auto patterns = Config::loadExtractionPatterns("test/test-extraction_patterns.yaml");
    ErrorObject obj;
    obj.entries.push_back({"VALUE", "my_file.py", 5, "error: 'variableName' is bad"}); 

    ProduceResolutions resolver(resolutions, patterns);
    auto alts = resolver.produce(obj);

    // VALUE (2 common + 2 fixes) = 4
    assert(alts.size() == 4);
    assert(alts[2].type == "DEFAULT_INITIALIZE");
    assert(alts[3].type == "NULL_INITIALIZE");
}

void test_produce_string_replacement() {
    auto resolutions = Config::loadResolutions("test/test-resolutions.yaml");
    auto patterns = Config::loadExtractionPatterns("test/test-extraction_patterns.yaml");
    ErrorObject obj;
    obj.entries.push_back({"TYPO", "style.css", 5, "error: unknown property 'color'"});

    ProduceResolutions resolver(resolutions, patterns);
    auto alts = resolver.produce(obj);

    assert(alts.size() == 3);
    assert(alts[2].type == "FIX_COLOUR");
    assert(alts[2].findText == "color");
    assert(alts[2].replaceText == "colour");
}

void test_produce_multiple_value_errors() {
    auto resolutions = Config::loadResolutions("resolutions.yaml");
    auto patterns = Config::loadExtractionPatterns("extraction_patterns.yaml");
    ErrorObject obj;
    obj.entries.push_back({"VALUE", "script1.js", 1, "error: 'varA' is undefined"});
    obj.entries.push_back({"VALUE", "script2.js", 3, "error: 'varB' is null"});

    ProduceResolutions resolver(resolutions, patterns);
    auto alts = resolver.produce(obj);

    // 2 * (2 common + 2 fixes) = 8
    assert(alts.size() == 8);
}
