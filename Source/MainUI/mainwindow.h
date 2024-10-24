#ifndef MAINWINDOW_H
#define MAINWINDOW_H




#include "../pch.h"
#include "event/ZoomableGraphicsView.h"
#include "statuslogs.h"
#include "PAUTFileReader/AscanProcessor.h"
#include "OmConnect/IOmConnect.h"
#include "SystemConfig/ConfigLocator.h"


namespace nmainUI {
    class UIFrame { 
    public:
        static UIFrame& getInstance() { static UIFrame instance; return instance; }
        int mainloop(int argc, char* argv[]);
    private:
        UIFrame();
        ~UIFrame() = default;
        nmainUI::statuslogs* sttlogs = nullptr;
        QApplication* app;

    };

}

#endif // MAINWINDOW_H
