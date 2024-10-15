#ifndef SETTINGPARA_H
#define SETTINGPARA_H

struct SettingParams {
	bool bhighResBscan = true;
    std::string ipAddress = "192.168.0.1";
    unsigned int port = 8080;
    unsigned int timeout = 5000;
    std::string device_name = "RoqView";

    std::string qsTheme = "Takezo.qss";
};

class SettingsManager {
public:
    // Getter for the Singleton instance
    static SettingsManager* getInstance() {
        if (!instance) {
            instance = new SettingsManager();
        }
        return instance;
    }

    const SettingParams& getSettings() const { return settings; }
    void updateSettings(const SettingParams& newSettings) { settings = newSettings; }

    void saveToRegistry();
    void loadFromRegistry();

    ~SettingsManager() {
        saveToRegistry();
    }

    // Delete copy constructor and assignment operator to ensure singleton
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

private:
    // Private constructor to ensure no external instantiation
    SettingsManager()  {
        loadFromRegistry();
    }

    static SettingsManager* instance;  // Singleton instance
    SettingParams settings;
};


#endif