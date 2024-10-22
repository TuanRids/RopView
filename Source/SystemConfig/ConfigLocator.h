#ifndef CONFIG_LOCATOR_H
#define CONFIG_LOCATOR_H
#include "../pch.h"
#include "SystemConfig.h"

class ConfigLocator {
private:
    ConfigLocator() {
        loadFromRegistry();  
    }

public:
    static ConfigLocator& getInstance() {
        static ConfigLocator instance;
        return instance;
    }

    static OmniConfig omconf;
    static SettingConfig settingconf;

    void saveToRegistry();
    void loadFromRegistry();
};


#endif