#include "SettingPara.h"



SettingsManager* SettingsManager::instance = nullptr;

// Implementation of save and load methods
void SettingsManager::saveToRegistry() {
    QSettings regSettings("RoqView COM", "Setting");
    regSettings.setValue("resolution", settings.bhighResBscan);
    regSettings.setValue("ipAddress", QString::fromStdString(settings.ipAddress));
    regSettings.setValue("port", settings.port);
    regSettings.setValue("timeout", settings.timeout);
    regSettings.setValue("device_name", QString::fromStdString(settings.device_name));
    regSettings.setValue("qsTheme", QString::fromStdString(settings.qsTheme));

}


void SettingsManager::loadFromRegistry() {
    QSettings regSettings("RoqView COM", "Setting");

    settings.bhighResBscan = regSettings.value("resolution", true).toBool();
    settings.ipAddress = regSettings.value("ipAddress", "192.168.0.1").toString().toStdString();
    settings.port = regSettings.value("port", 8080).toUInt();
    settings.timeout = regSettings.value("timeout", 5000).toUInt();
    settings.device_name = regSettings.value("device_name", "RoqView").toString().toStdString();
    settings.qsTheme = regSettings.value("qsTheme", "RoqView").toString().toStdString();

}
