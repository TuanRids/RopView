#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pch.h"
#include "IUIWindow.h"
class nFrame;
namespace nmainUI {
    class UIFrame : public UIWindow {
    public:
        static UIFrame& getInstance() { static UIFrame instance; return instance; }
        int mainloop(int argc, char* argv[]);
        void logical();
        void refreshxyz(nFrame* crframe) ;
    private:
        UIFrame();
        ~UIFrame() = default;   
    };

}

#endif // MAINWINDOW_H
