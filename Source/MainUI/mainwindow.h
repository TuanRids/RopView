#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "..\pch.h"


#include "..\Source\OmConnect\OmConnect.h"
#include "..\event\ZoomableGraphicsView.h"

#include "IUIWindow.h"

namespace nmainUI {
    /// <summary>
    /// MainUI with singleton for avoiding multiple instance.
    /// </summary>
    class UIFrame : public UIWindow {
    public:
        static UIFrame& getInstance() { static UIFrame instance; return instance; }
        int mainloop(int argc, char* argv[]);
        void logical();
        void refreshxyz(nFrame* crframe) ;
    private:
        UIFrame();
        ~UIFrame() = default;
        // Other object        
    };

}

#endif // MAINWINDOW_H
