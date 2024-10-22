#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "../pch.h"
#include "statuslogs.h"
#include "../Source/PAUTFileReader/AscanProcessor.h"
#include "../OmConnect/OmConnect.h"
#include "SystemConfig/ConfigLocator.h"
class UIWindow {
public:
    virtual ~UIWindow() = default;

    unsigned int get_resolution() const { return resolution; }
    bool check_isCscanLayer() const { return isCscanLayer; }

protected:
    static ConfigLocator* configL;

    static std::shared_ptr<IOmConnect> omc;
    static unsigned int resolution;
    static bool isCscanLayer;
    static nmainUI::statuslogs* sttlogs;
    static std::shared_ptr<AscanProcessor> processor;
    static std::unique_ptr<QSettings> settings;
    std::unique_ptr<QApplication> app;
};
// Define static variables outside the class declaration


#endif // UIWINDOW_H
