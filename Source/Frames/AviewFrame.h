#ifndef ASCANFRAME_H
#define ASCANFRAME_H

#include "../pch.h"
#include "PAUTFileReader/AscanProcessor.h"
#include "MainUI/FacObsFrames/FactoryMgr.h"
#include "MainUI/FacObsFrames/ObserverMgr.h"
#include "MainUI/mainwindow.h"

// Aview Frame
class AviewFrame : public nObserver {
private:
    //************** Method
    void OfflineProcess(); 
    void ReZoom(); 
    void RenderFrame();

    //************** Properties   
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<QGraphicsView> graphicsView;

    QValueAxis* axisX;
    QValueAxis* axisY;
    QVector<QPointF> points;

    QChart* chart;
    QSplineSeries* lineSeries;
    QChartView* chartView;
public:

    QWidget* createFrame() override;
    void update() override;
    void updateRealTime() override ;

};

#endif
