#include "ConfigLocator.h"

OmniConfig ConfigLocator::omconf = OmniConfig();
SettingConfig ConfigLocator::settingconf = SettingConfig();

void ConfigLocator::saveToRegistry() {
    QSettings regSettings("RoqView COM", "Setting");
    regSettings.setValue("resolution", settingconf.bhighResBscan);
    regSettings.setValue("ipAddress", QString::fromStdString(settingconf.ipAddress));
    regSettings.setValue("port", settingconf.port);
    regSettings.setValue("timeout", settingconf.timeout);
    regSettings.setValue("device_name", QString::fromStdString(settingconf.device_name));
    regSettings.setValue("qsTheme", QString::fromStdString(settingconf.qsTheme));
}

void ConfigLocator::loadFromRegistry() {
    QSettings regSettings("RoqView COM", "Setting");

    settingconf.bhighResBscan = regSettings.value("resolution", true).toBool();
    settingconf.ipAddress = regSettings.value("ipAddress", "192.168.0.1").toString().toStdString();
    settingconf.port = regSettings.value("port", 8080).toUInt();
    settingconf.timeout = regSettings.value("timeout", 5000).toUInt();
    settingconf.device_name = regSettings.value("device_name", "RoqView").toString().toStdString();
    settingconf.qsTheme = regSettings.value("qsTheme", "RoqView").toString().toStdString();
}