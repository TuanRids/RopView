#ifndef ASCANFRAME_H
#define ASCANFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\FactoryMgr.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"

// Aview Frame
class AviewFrame : public nObserver {
private:
    //************** Method
    void OfflineProcess(); // ofline
    void RealTimeProcess(); // ofline
    void RenderFrame();

    //************** Properties
    nmainUI::UIFrame* uiframe;
    unsigned int y_level_ = 0;
    unsigned int x_level_ = 0;
    
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<QGraphicsView> graphicsView;

    QValueAxis* axisX;
    QValueAxis* axisY;



    QChart* chart;
    QLineSeries* lineSeries;
    QChartView* chartView;
    spdlog::logger* sdk_logger;
public:

    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    QWidget* createFrame() override;
    void update() override;
    void updateRealTime() override ;
    ~AviewFrame() {
        if (graphicsView) {
            graphicsView->setScene(nullptr);
        }

        graphicsView.reset();
        scene.reset();
    }
};

#endif
