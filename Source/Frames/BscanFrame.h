#ifndef BSCANFRAME_H
#define BSCANFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"
#include "..\Source\event\ZoomableGraphicsView.h"
#include "..\MainUI\SettingPara.h"
// Graphics Frame
class BscanFrame : public nFrame, public nObserver {
private:
    void CreateYZScan();
    void MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView);
    void render_graphic();

    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;
    std::unique_ptr<cv::Mat> orgimage;
    std::unique_ptr<cv::Mat> scaledImage;
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);

    bool isPanningLocal = false;
    QGraphicsView* navigatorView;
public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;
};

#endif
