#include "glia_prune_cmd.h"
#include "../cli/cli.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>
#include <filesystem>

namespace glia::app {

namespace {
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
}

glia::core::CommandResult PruneCurrentCommand::execute(const std::vector<std::string>& args) {
    std::string untracked = exec("git ls-files --others --exclude-standard");
    std::stringstream ss(untracked);
    std::string item;
    int deletedCount = 0;
    
    while (std::getline(ss, item)) {
        if (item.empty()) continue;
        if (glia::cli::Prompter::confirm("Delete " + item + "?")) {
            try {
                if (std::filesystem::is_directory(item)) {
                    std::filesystem::remove_all(item);
                } else {
                    std::filesystem::remove(item);
                }
                std::cout << "Deleted: " << item << std::endl;
                deletedCount++;
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error deleting " << item << ": " << e.what() << std::endl;
            }
        }
    }
    
    return {glia::core::ExitCode::Success, "Pruned " + std::to_string(deletedCount) + " untracked items"};
}

}
