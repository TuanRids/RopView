#ifndef MAINWINDOW_H
#define MAINWINDOW_H




#include "../pch.h"
#include "event/ZoomableGraphicsView.h"
#include "statuslogs.h"
#include "PAUTFileReader/AscanProcessor.h"
#include "OmConnect/IOmConnect.h"
#include "SystemConfig/ConfigLocator.h"

class nSubject;
namespace nmainUI {
    class UIFrame { 
    public:
        static UIFrame& getInstance() { static UIFrame instance; return instance; }
        int mainloop(int argc, char* argv[]);
    private:
        UIFrame();
        ~UIFrame();
        nmainUI::statuslogs* sttlogs = nullptr;
        QApplication* app;
        std::shared_ptr<nSubject> nsubject;
    };

}

#endif // MAINWINDOW_H
