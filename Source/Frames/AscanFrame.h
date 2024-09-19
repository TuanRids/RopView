#ifndef ASCANFRAME_H
#define ASCANFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"

// Ascan Frame
class AscanFrame : public nFrame, public nObserver {
private:
    //************** Method
    std::shared_ptr<QImage> CreateXZScan();
    void CreateAScan();

    //************** Properties
    nmainUI::UIFrame* uiframe;
    unsigned int y_level_ = 0;
    unsigned int x_level_ = 0;
    
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<QGraphicsView> graphicsView;
    QChart* chart;
    QLineSeries* lineSeries;
    QChartView* chartView;
public:

    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;


};

#endif
