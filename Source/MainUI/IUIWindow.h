#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "pch.h"
#include <memory>
#include <QSettings>
#include <QApplication>
#include "statuslogs.h"
namespace nmainUI {
    class statuslogs;
}
class AscanProcessor;
class IOmConnect;
class nSubject;

class UIWindow {
public:
    virtual ~UIWindow() = default;

    unsigned int get_resolution() const { return resolution; }
    bool check_isCscanLayer() const { return isCscanLayer; }

protected:
    static std::shared_ptr<IOmConnect> omc;
    static unsigned int resolution;
    static bool isCscanLayer;
    static nmainUI::statuslogs* sttlogs;
    static std::shared_ptr<AscanProcessor> processor;
    static std::unique_ptr<QSettings> settings;
    std::unique_ptr<QApplication> app;
    std::shared_ptr<nSubject> nsubject;
};

// Define static variables outside the class declaration


#endif // UIWINDOW_H
