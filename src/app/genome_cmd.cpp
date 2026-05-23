#include "genome_cmd.h"
#include "../util/translator.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace glia::app {
glia::core::CommandResult GenomeCommand::execute(const std::vector<std::string>& args) {
    using glia::util::Translator;
    if (args.size() < 2) return {glia::core::ExitCode::UsageError, Translator::t("msg_error")};

    std::string query = args[1];
    std::string cacheDir = m_meta.params.count("cache_dir") ? m_meta.params.at("cache_dir") : "cache";
    std::string prefix = m_meta.params.count("filename_prefix") ? m_meta.params.at("filename_prefix") : "";
    std::string suffix = m_meta.params.count("filename_suffix") ? m_meta.params.at("filename_suffix") : ".json";

    std::string cacheFile = cacheDir + "/" + prefix + query + suffix;

    if (!std::filesystem::exists(cacheFile)) {
        std::filesystem::create_directories(cacheDir);
        std::ofstream out(cacheFile);
        out << m_meta.params["mock_payload"];
        out.close();
    }

    return {glia::core::ExitCode::Success, Translator::t("msg_done")};
}
}
