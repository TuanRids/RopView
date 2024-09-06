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
    std::shared_ptr<QImage> CreateXYScan();

    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    nmainUI::UIFrame* uiframe;

    unsigned int z_level_ = 0;

public:
    void setUIFrame(nmainUI::UIFrame* ui);
    QWidget* createFrame() override;
    void update() override;
};

#endif
