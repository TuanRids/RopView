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

    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;
    uint64_t ysize, xsize, zsize;

    unsigned int z_level_ = 0;

public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;
    void setter_Curpt(int x, int y, int z) {curpt.x = x; curpt.y = y; curpt.z = z;}
};

#endif
