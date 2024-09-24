#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "..\pch.h"
#include "statuslogs.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"

class UIWindow {
public:
    virtual ~UIWindow() = default;

    unsigned int get_resolution() const { return resolution; }
    bool check_isCscanLayer() const { return isCscanLayer; }

protected:
    static unsigned int resolution;
    static bool isCscanLayer;
    static nmainUI::statuslogs* sttlogs;
    static std::shared_ptr<AscanProcessor> processor;
    static std::unique_ptr<QSettings> settings;
};
// Define static variables outside the class declaration


#endif // UIWINDOW_H
