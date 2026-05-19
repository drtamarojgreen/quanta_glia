#ifndef GLIA_STORAGE_STORAGE_H
#define GLIA_STORAGE_STORAGE_H
#include <string>
#include <filesystem>

namespace glia::storage {
class Storage {
public:
    static bool init(const std::string& kb, const std::string& cache);
};
}
#endif
