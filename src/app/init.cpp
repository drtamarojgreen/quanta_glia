#include "init.h"
#include "../storage/storage.h"
namespace glia::app {
glia::core::CommandResult InitCommand::execute(const std::vector<std::string>& args) {
    if (glia::storage::Storage::init("./knowledge_base", "./repo_cache")) {
        return {glia::core::ExitCode::Success, "Workspace initialized"};
    }
    return {glia::core::ExitCode::FilesystemError, "Failed to initialize workspace"};
}
}
