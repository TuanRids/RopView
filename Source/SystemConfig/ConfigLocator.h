#ifndef CONFIG_LOCATOR_H
#define CONFIG_LOCATOR_H
#include "../pch.h"
#include "SystemConfig.h"


/// ConfigLocator; Only in MainThread.
/// Contain: 
///     SettingConfig (OMNI Connect); 
///     SystemParams (Settings Popup); 
///     VisualizeConfig (Scan Results).
//TODO: Create new thread for rendering, connect directly into readData for avoiding slowing mainUI.
class ConfigLocator {
private:
    ConfigLocator() {
		settingconf = std::make_shared<SettingConfig>();
        sysParams = std::make_shared<SystemParams>();
        visualConfig = std::make_shared<VisualizeConfig>();
        loadFromRegistry();  
    }

public:
    static ConfigLocator& getInstance() {
        static ConfigLocator instance;
        return instance;
    }
    static std::shared_ptr<SettingConfig> settingconf; /* Setting Config*/
    static std::shared_ptr<SystemParams> sysParams; /* System Params*/
    static std::shared_ptr<VisualizeConfig> visualConfig; /* Visualize Config; Used To store all setting for mainThread*/
    void saveToRegistry();
    void loadFromRegistry();
};

/// Contain all Setting for Omniscan;
/// Have to be used in 2 threads: 
///     MainThread and Reading AcquisitionThread.
// TODO: Work Directly with VisualizeThread and MainThread.
class OmSetupL {
private:
    OmSetupL() {
        OMS = std::make_shared<Om_Settup_Config>();
    }
public:
    static OmSetupL& getInstance() {
        static OmSetupL instance;
        return instance;
    }
    static std::shared_ptr<Om_Settup_Config> OMS; /* Om Setup Configuration */
};

/// Used as a global variable to contain Scan Results
/// Processing data and visualizing the data.
//TODO: GPU???
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
    void resetASscan() {
        SViewBuf.reset();  AViewBuf.reset();
        SViewBuf = std::make_shared<cv::Mat>();
        AViewBuf = std::make_shared<QVector<QPointF>>();
    }
    void resetAll()
    {
        if (SViewBuf) SViewBuf.reset(); SViewBuf = std::make_shared<cv::Mat>();
		if (BViewBuf) BViewBuf.reset(); BViewBuf = std::make_shared<cv::Mat>(cv::Mat());
		if (CViewBuf) CViewBuf.reset(); CViewBuf = std::make_shared<cv::Mat>(cv::Mat());
		if (AViewBuf) AViewBuf.reset(); AViewBuf = std::make_shared<QVector<QPointF>>();		
    }
    std::shared_ptr<cv::Mat> SViewBuf;
    std::shared_ptr<cv::Mat> BViewBuf;
    std::shared_ptr<cv::Mat> CViewBuf;
    std::shared_ptr<QVector<QPointF>> AViewBuf;
};


#endif