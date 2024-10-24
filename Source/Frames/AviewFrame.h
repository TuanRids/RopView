#ifndef ASCANFRAME_H
#define ASCANFRAME_H

#include "../pch.h"
#include "../Source/PAUTFileReader/AscanProcessor.h"
#include "../Source/MainUI/FactoryMgr.h"
#include "../Source/MainUI/ObserverMgr.h"
#include "../Source/MainUI/mainwindow.h"

// Aview Frame
class AviewFrame : public nObserver {
private:
    //************** Method
    void OfflineProcess(); 
    void RealTimeProcess(); 
    void RenderFrame();

    //************** Properties   
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<QGraphicsView> graphicsView;

    QValueAxis* axisX;
    QValueAxis* axisY;

    QChart* chart;
    QLineSeries* lineSeries;
    QChartView* chartView;
public:

    QWidget* createFrame() override;
    void update() override;
    void updateRealTime() override ;

};

#endif
