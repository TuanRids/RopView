#ifndef CVIEWFRAME_H
#define CVIEWFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"
#include "..\Source\event\ZoomableGraphicsView.h"


// Graphics Frame
class CviewFrame : public nObserver {
private:

    void CreateXYview();
    void MouseGetPosXY();
    void addPoints(bool Cviewlink, int x, int y);

    QGraphicsScene* scene;
    ZoomableGraphicsView* graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);

    std::shared_ptr<XYOverlayGrid> overlay = nullptr;
    std::shared_ptr<cv::Mat> orgimage;
    std::shared_ptr<cv::Mat> scaledImage;
    QGraphicsView* navigatorView;
    bool isRealTime = false;

public:
    QWidget* createFrame() override;
    void update() override;
    void updateRealTime() override;
    void setter_Curpt(int x, int y, int z) {curpt.x = x; curpt.y = y; curpt.z = z;}
    ~CviewFrame() {
        if (graphicsView) {
            graphicsView->setScene(nullptr);
        }
    }
};

#endif
