#ifndef NGRAPHICFRAME_H
#define NGRAPHICFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"
#include "..\Source\event\ZoomableGraphicsView.h"


// Graphics Frame
class CscanFrame : public nFrame, public nObserver {
private:
    void CreateXYScan();
    void MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView);
    
    void render_graphic();
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);

    std::unique_ptr<cv::Mat> orgimage;
    std::unique_ptr<cv::Mat> scaledImage;
    QGraphicsView* navigatorView;
    bool isLocalPanning = false;
public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;
    void setter_Curpt(int x, int y, int z) {curpt.x = x; curpt.y = y; curpt.z = z;}
};

#endif
