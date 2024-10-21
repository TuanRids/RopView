#ifndef IAFRAME_H
#define IAFRAME_H

#include "..\pch.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\Source\MainUI\ObserverMgr.h"
#include "..\Source\MainUI\mainwindow.h"


class IAFrame : public nFrame {
protected:
    //************** Method
    virtual void CreateAview() = 0;

    //************** Properties
    nmainUI::UIFrame* uiframe;
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<QGraphicsView> graphicsView;
    QChart* chart;
    QLineSeries* lineSeries;
    QChartView* chartView;

    QValueAxis* axisX;
    QValueAxis* axisY;
public:
    void setUIFrame(nmainUI::UIFrame* ui) { uiframe = ui; }
    void UISetup() {
        // ********** SETTINGS **********
        axisX = new QValueAxis();
        axisY = new QValueAxis();
        static QFont axisFont;

        // Pen for line series
        static QPen linePen(QColor(0, 102, 204));

        if (!chart || !lineSeries) {
            axisFont.setPointSize(8);
            linePen.setWidth(2);
            scene->clear();
            chart = new QChart();
            lineSeries = new QLineSeries();

            // Attach series and axes
            chart->addSeries(lineSeries);
            chart->legend()->hide();
            chart->addAxis(axisX, Qt::AlignTop);
            chart->addAxis(axisY, Qt::AlignLeft);
            lineSeries->attachAxis(axisX);
            lineSeries->attachAxis(axisY);
            lineSeries->setPen(linePen);
            lineSeries->setPointsVisible(false);

            // Axis settings
            axisX->setTitleBrush(QBrush(QColor(Qt::darkCyan)));
            axisY->setTitleBrush(QBrush(QColor(Qt::darkCyan)));
            axisY->setLabelsColor(QColor(Qt::cyan));
            axisX->setLabelsFont(axisFont);
            axisY->setLabelsFont(axisFont);
            axisX->setLabelsColor(QColor(Qt::cyan));
            axisX->setGridLineColor(QColor(80, 80, 80));
            axisY->setGridLineColor(QColor(80, 80, 80));
            axisX->setLabelFormat("%02i");
            axisX->setTickCount(10);
            axisY->setTickCount(10);

            // Chart settings
            chart->setBackgroundBrush(Qt::NoBrush);
            chart->setMargins(QMargins(0, 0, 0, 0));
            chartView = new QChartView(chart);
            chartView->setStyleSheet("background: transparent");
            chartView->setAttribute(Qt::WA_TranslucentBackground);
            chartView->setRenderHint(QPainter::Antialiasing, true);
            chartView->setMinimumSize(1900, 1080);
            chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            chartView->setRubberBand(QChartView::RectangleRubberBand);

            scene->addWidget(chartView);
        }
    }
    QWidget* createFrame() {
        scene = std::make_shared<QGraphicsScene>();
        graphicsView = std::make_shared<QGraphicsView>();
        graphicsView->setScene(&*scene);
        graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
        graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QVBoxLayout* layout = new QVBoxLayout();
        QWidget* frame = new QWidget();
        layout->addWidget(graphicsView.get());
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        frame->setLayout(layout);

        return frame;
    }
};

#endif
