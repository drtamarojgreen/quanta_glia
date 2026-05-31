#ifndef GLIA_UTIL_BENCHMARK_UTILS_H
#define GLIA_UTIL_BENCHMARK_UTILS_H

#include <chrono>
#include <map>
#include <string>

namespace glia::util {

class BenchmarkTimer {
public:
    static void start(const std::string& name) {
        m_starts[name] = std::chrono::steady_clock::now();
    }

    static double stop(const std::string& name) {
        if (m_starts.count(name)) {
            auto end = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - m_starts[name]).count();
            m_results[name] = diff / 1000.0;
            return m_results[name];
        }
        return 0.0;
    }

    static double get(const std::string& name) {
        return m_results.count(name) ? m_results[name] : 0.0;
    }

private:
    static inline std::map<std::string, std::chrono::steady_clock::time_point> m_starts;
    static inline std::map<std::string, double> m_results;
};

}

#endif
