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


        void logical(nFrame* frame);
        void refreshxyz(const int x, const int y, const int z);

    private:
        UIFrame();
        ~UIFrame() { };
        
        // UI settings
        void UISETTING();

        // Disable copy constructor and assignment operator
        UIFrame(const UIFrame&) = delete;
        UIFrame& operator=(const UIFrame&) = delete;

        // settings
        QPixmap loadLogoFromResource();
        
        // Main properties
        QApplication* app;
        QMainWindow* mainWindow;
        QWidget* centralWidget;
        QPushButton* button;
        QGraphicsView* graphicsView;
        QGraphicsScene* scene;
        QSplitter* splitter;
        
        //logs
        statuslogs* sttlogs;
        // Other object
        std::shared_ptr<AscanProcessor> processor;

    };
}

#endif // MAINWINDOW_H
