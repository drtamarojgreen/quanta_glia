#ifndef GLIA_CONFIG_MANAGER_H
#define GLIA_CONFIG_MANAGER_H

#include "glia_config.h"
#include <QString>

class GliaConfigManager {
public:
    static bool saveToXml(const GliaConfig &config, const QString &filePath);
    static bool loadFromXml(GliaConfig &config, const QString &filePath);

    static bool saveToJson(const GliaConfig &config, const QString &filePath);
    static bool loadFromJson(GliaConfig &config, const QString &filePath);
};

#endif // GLIA_CONFIG_MANAGER_H
