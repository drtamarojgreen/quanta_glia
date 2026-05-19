#ifndef GLIA_CORE_RESULT_H
#define GLIA_CORE_RESULT_H
#include <string>
#include <vector>

namespace glia::core {
enum class ExitCode {
    Success = 0,
    UsageError = 1,
    ConfigError = 2,
    FilesystemError = 3,
    PolicyDenial = 4,
    PartialSuccess = 5,
    InternalFailure = 6
};

struct CommandResult {
    ExitCode code;
    std::string userMessage;
    std::string detailMessage;
    std::vector<std::string> hints;
};
}
#endif
