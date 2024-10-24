#ifndef CONFIG_LOCATOR_H
#define CONFIG_LOCATOR_H
#include "../pch.h"
#include "SystemConfig.h"

class ConfigLocator {
private:
    ConfigLocator() {
        omconf = std::make_shared<OmniConfig>();
		settingconf = std::make_shared<SettingConfig>();
        sysParams = std::make_shared<SystemParams>();
        loadFromRegistry();  
    }

public:
    static ConfigLocator& getInstance() {
        static ConfigLocator instance;
        return instance;
    }

    static std::shared_ptr<OmniConfig> omconf;
    static std::shared_ptr<SettingConfig> settingconf;
    static std::shared_ptr<SystemParams> sysParams;

    void saveToRegistry();
    void loadFromRegistry();
};


#endif