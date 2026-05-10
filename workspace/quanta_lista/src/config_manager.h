#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QString>
#include <vector>
#include "task.h"

class ConfigManager {
public:
    static bool saveToXml(const QString& filePath, const std::vector<Task>& tasks);
    static bool loadFromXml(const QString& filePath, std::vector<Task>& tasks);

    static bool saveToJson(const QString& filePath, const std::vector<Task>& tasks);
    static bool loadFromJson(const QString& filePath, std::vector<Task>& tasks);
};

#endif // CONFIG_MANAGER_H
