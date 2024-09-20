#ifndef BSCANFRAME_H
#define BSCANFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"
#include "..\Source\event\ZoomableGraphicsView.h"

// Graphics Frame
class BscanFrame : public nFrame, public nObserver {
private:
    std::shared_ptr<QImage> CreateYZScan();
    void MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView);

    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;
    std::unique_ptr<cv::Mat> orgimage;
    std::unique_ptr<cv::Mat> scaledImage;


    unsigned int x_level_ = 0;
public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;
};

#endif
