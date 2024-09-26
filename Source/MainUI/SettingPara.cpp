#include "SettingPara.h"



SettingsManager* SettingsManager::instance = nullptr;

// Implementation of save and load methods
void SettingsManager::saveToRegistry() {
    QSettings settings("RoqView COM", "Setting");
    settings.setValue("resolution", bhighResBscan);
}

void SettingsManager::loadFromRegistry() {
    QSettings settings("RoqView COM", "Setting");
    bhighResBscan = settings.value("resolution", true).toBool();
}