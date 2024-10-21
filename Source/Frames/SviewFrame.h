#ifndef BSCANFRAME_H
#define BSCANFRAME_H

#include "pch.h"
#include "PAUTFileReader\AscanProcessor.h"
#include "MainUI\ObserverMgr.h"
#include "MainUI\mainwindow.h"
#include "event\ZoomableGraphicsView.h"
// Graphics Frame
class SviewFrame : public nFrame, public nObserver {
private:
    void CreateYZview();
    void MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView);
    void addPoints(bool Cviewlink, int x, int y);
    std::shared_ptr<XYOverlayGrid> overlay;
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;
    std::shared_ptr<cv::Mat> orgimage;
    std::shared_ptr<cv::Mat> scaledImage;
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);

    QGraphicsView* navigatorView;
public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame();
    void update() override;
};

#endif
