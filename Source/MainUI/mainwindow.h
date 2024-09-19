#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "..\pch.h"


#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\OmConnect\OmConnect.h"

#include "..\event\ZoomableGraphicsView.h"

namespace nmainUI {
    /// <summary>
    /// MainUI with singleton for avoiding multiple instance.
    /// </summary>
    class UIFrame {
    public:
        static UIFrame& getInstance() { static UIFrame instance; return instance; }
        int mainloop(int argc, char* argv[]);


        void logical();
        void refreshxyz();

    private:
        UIFrame();
        ~UIFrame() { };
        
        // UI settings
        void UISETTING(QApplication* app);

        // Disable copy constructor and assignment operator
        UIFrame(const UIFrame&) = delete;
        UIFrame& operator=(const UIFrame&) = delete;

        // settings
        QPixmap loadLogoFromResource();
        
        //logs
        statuslogs* sttlogs;
        // Other object
        std::shared_ptr<AscanProcessor> processor;


        //
        QWidget* createLogFrame();
        QWidget* createAscanFrame();
        QWidget* createCScanBScanFrame();
    };
}

#endif // MAINWINDOW_H
