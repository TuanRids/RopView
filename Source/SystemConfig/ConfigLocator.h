#ifndef CONFIG_LOCATOR_H
#define CONFIG_LOCATOR_H
#include "../pch.h"
#include "SystemConfig.h"
// ============= ConFig =============
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


// ============ UIArtScan =============
class UIArtScan {
private:
    UIArtScan() {
        SViewBuf = std::make_shared<cv::Mat>();
        BViewBuf = std::make_shared<cv::Mat>(cv::Mat());
        CViewBuf = std::make_shared<cv::Mat>(cv::Mat());
        AViewBuf = std::make_shared<QVector<QPointF>>();
    }

public:
    static UIArtScan& getInstance() {
        static UIArtScan instance;
        return instance;
    }
    void resetall() {
        SViewBuf.reset();  AViewBuf.reset();
        SViewBuf = std::make_shared<cv::Mat>();
        AViewBuf = std::make_shared<QVector<QPointF>>();
    }
    std::shared_ptr<cv::Mat> SViewBuf;
    std::shared_ptr<cv::Mat> BViewBuf;
    std::shared_ptr<cv::Mat> CViewBuf;
    std::shared_ptr<QVector<QPointF>> AViewBuf;
};


#endif