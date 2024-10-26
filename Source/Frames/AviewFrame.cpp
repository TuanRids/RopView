#include "AviewFrame.h"
#include "..\pch.h"
#include <omp.h>
#include <random>
#include "SystemConfig/ConfigLocator.h"

int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
QWidget* AviewFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<QGraphicsView>();
    graphicsView->setScene(scene.get());
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
    try {
        if (nAscanCollection.empty()) return;
        if (!lineSeries)  RenderFrame(); 
        points = *ArtScan->AViewBuf;
        size_t dataSize = points.size();

        lineSeries->clear();  
        lineSeries->replace(points); 
        if (!axisX)
        {
            axisY->setRange(0, static_cast<int>(dataSize));
            axisX->setRange(
                std::min_element(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) { return a.x() < b.x(); })->x(),
                std::max_element(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) { return a.x() < b.x(); })->x()
            );
        }
        axisY->setReverse(true);
        static size_t lastpos = curpt.y;
        if (ConfigLocator::getInstance().omconf->BeamPosition != lastpos)
        {
            lastpos = ConfigLocator::getInstance().omconf->BeamPosition;
            if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
            sttlogs->logInfo("Beam Position: " + std::to_string(lastpos));
        }
 
    }
    catch (exception& e)
    { std::cout << "ESCAN ERROR: " << e.what() << std::endl; return; }
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
        static int lastpos[3] = { curpt.x, curpt.y, curpt.z };
        if (curpt.x != lastpos[0] || curpt.y != lastpos[1] || curpt.z != lastpos[2])
        {
			lastpos[0] = curpt.x; lastpos[1] = curpt.y; lastpos[2] = curpt.z;
            sttlogs->logInfo("Coord x: " + std::to_string(curpt.x) + " - Coord y: " + std::to_string(curpt.y) + " Coord z: " + std::to_string(curpt.z) + ".");
        }
    }
}




void AviewFrame::RenderFrame()
{
    // ********** INITIALIZATION & SETTINGS **********
    static QFont axisFont;
    static QPen linePen(QColor(0, 102, 204));

    if (!chart || !lineSeries) {
        axisX = new QValueAxis();
        axisY = new QValueAxis();       
        axisFont.setPointSize(8);
        linePen.setWidth(2);
        scene->clear();
        chart = new QChart();
        lineSeries = new QSplineSeries();

        // Attach series and axes
        chart->addSeries(lineSeries);
        chart->legend()->hide();
        chart->addAxis(axisX, Qt::AlignTop);
        chart->addAxis(axisY, Qt::AlignLeft);
        lineSeries->attachAxis(axisX);
        lineSeries->attachAxis(axisY);
        lineSeries->setPen(linePen);
        lineSeries->setPointsVisible(true);

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
        QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_R), chartView);
        QObject::connect(shortcut, &QShortcut::activated, [this]() {
            size_t dataSize = points.size();
            axisY->setRange(0, static_cast<int>(dataSize));
            axisX->setRange(
                std::min_element(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) { return a.x() < b.x(); })->x(),
                std::max_element(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) { return a.x() < b.x(); })->x()
            );
            });
    }
    cout << "Hekllo";
    // Adjust aspect ratio based on the graphicsView's size
    int viewWidth = graphicsView->width() * 1.0;
    int viewHeight = graphicsView->height() * 1.0;
    double aspectRatio = static_cast<double>(viewWidth) / static_cast<double>(viewHeight);
    chartView->setFixedSize(viewWidth, viewHeight);
    chartView->setGeometry(-10, 10, viewWidth, viewHeight);
    graphicsView->update();
}



