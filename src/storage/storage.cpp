#include "storage.h"
namespace fs = std::filesystem;
namespace glia::storage {
bool Storage::init(const std::string& kb, const std::string& cache) {
    fs::create_directories(kb);
    fs::create_directories(cache);
    fs::create_directories("repo_archive");
    return true;
}
}
