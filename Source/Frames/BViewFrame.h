#ifndef BVIEWFRAME_H
#define BVIEWFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"
#include "..\Source\event\ZoomableGraphicsView.h"


// Graphics Frame
class BviewFrame : public nObserver {
private:
    void CreateCoordinate();
    void MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView);
    void addPoints(bool Cviewlink, int x, int y);

    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);

    std::shared_ptr<XYOverlayGrid> overlay;
    std::shared_ptr<cv::Mat> orgimage;
    std::shared_ptr<cv::Mat> scaledImage;
    QGraphicsView* navigatorView;
public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;
    void updateRealTime() override {};
    //void setter_Curpt(int x, int y, int z) { curpt.x = x; curpt.y = y; curpt.z = z; }
};

#endif
