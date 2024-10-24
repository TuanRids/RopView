#include "ConfigLocator.h"

// ========== ConFigLocator ==========
std::shared_ptr<OmniConfig> ConfigLocator::omconf = nullptr;
std::shared_ptr<SettingConfig> ConfigLocator::settingconf = nullptr;
std::shared_ptr<SystemParams> ConfigLocator::sysParams = nullptr;

void ConfigLocator::saveToRegistry() {
    QSettings regSettings("RoqView COM", "Setting");
    regSettings.setValue("resolution", settingconf->bhighResBscan);
    regSettings.setValue("ipAddress", QString::fromStdString(settingconf->ipAddress));
    regSettings.setValue("port", settingconf->port);
    regSettings.setValue("timeout", settingconf->timeout);
    regSettings.setValue("device_name", QString::fromStdString(settingconf->device_name));
    regSettings.setValue("qsTheme", QString::fromStdString(settingconf->qsTheme));
}

void ConfigLocator::loadFromRegistry() {
    QSettings regSettings("RoqView COM", "Setting");

    settingconf->bhighResBscan = regSettings.value("resolution", true).toBool();
    settingconf->ipAddress = regSettings.value("ipAddress", "192.168.0.1").toString().toStdString();
    settingconf->port = regSettings.value("port", 8080).toUInt();
    settingconf->timeout = regSettings.value("timeout", 5000).toUInt();
    settingconf->device_name = regSettings.value("device_name", "RoqView").toString().toStdString();
    settingconf->qsTheme = regSettings.value("qsTheme", "RoqView").toString().toStdString();
}

// ========== UIArtScan ===========
std::shared_ptr<cv::Mat> UIArtScan::SViewBuf = nullptr;
std::shared_ptr<cv::Mat> UIArtScan::BViewBuf = nullptr;
std::shared_ptr<cv::Mat> UIArtScan::CViewBuf = nullptr;
std::shared_ptr<QVector<QPointF>> UIArtScan::AViewBuf = nullptr;