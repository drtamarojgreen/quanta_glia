#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <map>
#include "../cpp/util/fact_utils.h"

namespace fs = std::filesystem;
using namespace Sorrel::Sdd::Util;

// @Card: config_loading_verification
// @Is python_available == 1
// @Results config_loading_operational == 1
void config_loading_verification_card(const std::map<std::string, std::string>& facts) {
    fs::path temp_root("tests/temp");
    std::string config_file_name = "test_config.yaml";
    fs::path config_file = temp_root / config_file_name;
    {
        std::ofstream f(config_file);
        f << "main:\n";
        f << "  knowledge_base: \"./test_kb\"\n";
        f << "  max_repos: 5\n";
    }

    std::string python_cmd = "python3 -c \"from scripts.utils import load_config; cfg = load_config('" + config_file.string() + "'); print(cfg['main']['max_repos'] == 5 and cfg['main']['knowledge_base'] == './test_kb')\"";

    FILE* pipe = popen(python_cmd.c_str(), "r");
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    int operational = (result.find("True") != std::string::npos) ? 1 : 0;
    std::cout << "config_loading_operational = " << operational << std::endl;

    fs::remove(config_file);
}

// @Card: json_logging_verification
// @Is python_available == 1
// @Results json_logging_operational == 1
void json_logging_verification_card(const std::map<std::string, std::string>& facts) {
    fs::path temp_root("tests/temp");
    std::string log_file_name = "test_json.log";
    fs::path log_path = temp_root / log_file_name;
    if (fs::exists(log_path)) fs::remove(log_path);

    std::string python_cmd = "python3 -c \"from scripts.utils import setup_logger; logger = setup_logger('" + log_path.string() + "'); logger.info('test message')\"";
    std::system(python_cmd.c_str());

    int operational = 0;
    if (fs::exists(log_path)) {
        std::ifstream f(log_path);
        std::string line;
        if (std::getline(f, line)) {
            if (line.find("\"message\": \"test message\"") != std::string::npos && line.find("\"level\": \"INFO\"") != std::string::npos) {
                operational = 1;
            }
        }
    }

    std::cout << "json_logging_operational = " << operational << std::endl;
    if (fs::exists(log_path)) fs::remove(log_path);
}

int main(int argc, char* argv[]) {
    auto facts = FactReader::readFacts("tests/sdd/facts/environment.facts");
    if (argc > 1 && std::string(argv[1]) == "logging") {
        json_logging_verification_card(facts);
    } else {
        config_loading_verification_card(facts);
    }
    return 0;
}
