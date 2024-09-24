#ifndef SETTINGPARA_H
#define SETTINGPARA_H

class SettingsManager {
public:
    // Getter for the Singleton instance
    static SettingsManager* getInstance() {
        if (!instance) {
            instance = new SettingsManager();
        }
        return instance;
    }

    // Getters
    bool getResolutionBscan() const { return highResolutionBscan; }

    // Setters
    void setResolutionBscan(bool value) { highResolutionBscan = value; }

    // Load/Save settings to registry (Windows specific)
    void saveToRegistry();
    void loadFromRegistry();

    // Destructor to clean up the instance
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
    // variables
    bool highResolutionBscan ;
};


#endif