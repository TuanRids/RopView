#include "AviewFrame.h"
#include "..\pch.h"
#include "OmConnect/CircularBuffer.h"

#include <random>

int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
QWidget* AviewFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<QGraphicsView>();
    graphicsView->setScene(&*scene);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // y_level_/ x_level_

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->addWidget(graphicsView.get());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0); 
    frame->setLayout(layout);

    return frame;
}

void AviewFrame::update() 
{

    if (scandat.Amplitudes.empty()) {
        if (sttlogs) { sttlogs->logWarning("No amplitude data available"); }
        return;
    }
    OfflineProcess();
    RenderFrame();
}

void AviewFrame::updateRealTime()
{
    static int idex = 0;
    if (sharedBuffer.size() == 0) {
        if (sttlogs) {
            sttlogs->logWarning("No data in shared buffer for real-time processing");
        }
        return;
    }

    auto data = sharedBuffer.pop();

    if (!lineSeries) RenderFrame(); 
    lineSeries->clear(); 
    QVector<QPointF> points;
    points.reserve(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        points.append(QPointF(data[i], static_cast<double>(i)));
    }

    lineSeries->replace(points);  
    axisX->setRange(*std::min_element(data.begin(), data.end()), *std::max_element(data.begin(), data.end()));
    axisY->setRange(0, data.size());  
    axisY->setReverse(true);  
    RenderFrame();
    std::cout << ("Real-time processing done: " + std::to_string(++idex));
}



void AviewFrame::OfflineProcess()
{
    // ********** PARAMETER VALIDATION **********
    if (scandat.Amplitudes.empty()) {
        if (sttlogs) {
            sttlogs->logWarning("No amplitude data available");
        }
        return;
    }

    // ********** PARAMETER PROCESSING **********
    if (!lineSeries) RenderFrame();
    lineSeries->clear();
    uint64_t zsize = scandat.AmplitudeAxes[0].Quantity;
    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    QVector<QPointF> points;
    points.reserve(zsize);

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();

    for (uint64_t z = 0; z < zsize; ++z) {
        uint64_t index = z * (xsize * ysize) + curpt.y * xsize + curpt.x;

        if (index >= scandat.Amplitudes.size()) {
            sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
            return;
        }

        int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
        double percentAmplitude = samplingAmplitude / (32768 / 100.0);

        minY = std::min(minY, percentAmplitude);
        maxY = std::max(maxY, percentAmplitude);
        points.append(QPointF(percentAmplitude, z));
    }

    lineSeries->replace(points);

    axisX->setRange(minY, 100);
    axisY->setRange(0, zsize);

    axisY->setReverse(true);
    // Logging coordinates
    if (!isPanning)
    {
        sttlogs->logInfo("Coord x: " + std::to_string(curpt.x) + " - Coord y: " + std::to_string(curpt.y) + " Coord z: " + std::to_string(curpt.z) + ".");
    }
}

void AviewFrame::RealTimeProcess()
{
}

void AviewFrame::RenderFrame()
{
    // ********** INITIALIZATION & SETTINGS **********
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    static QFont axisFont;
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
        axisY->setLabelsColor(QColor(Qt::cyan));
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
        chartView->setRenderHint(QPainter::Antialiasing, true);

        scene->addWidget(chartView);
    }

    // Adjust aspect ratio based on the graphicsView's size
    int viewWidth = graphicsView->width() * 1.0;
    int viewHeight = graphicsView->height() * 1.0;
    double aspectRatio = static_cast<double>(viewWidth) / static_cast<double>(viewHeight);
    chartView->setFixedSize(viewWidth, viewHeight);
    chartView->setGeometry(-10, 10, viewWidth, viewHeight);

    graphicsView->update();
}



