#include "AscanFrame.h"

QWidget* AscanFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<QGraphicsView>();
    graphicsView->setScene(&*scene);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->addWidget(graphicsView.get());
    frame->setLayout(layout);

    return frame;
}

void AscanFrame::update() {

    /*if (!curpt.CheckIdx(-1, y_level_, -1) && !curpt.CheckIdx(x_level_, -1, -1)) {
        return;
    }*/
    if (scandat.Amplitudes.empty()) {
        if (sttlogs) { sttlogs->logWarning("No amplitude data available"); }
        return;
    }
    CreateAScan();
    /*if (nimg && !nimg->isNull()) {
        QPixmap pixmap = QPixmap::fromImage(*nimg);
        scene->clear();
        scene->addPixmap(pixmap);
        graphicsView->update();
    }*/
    sttlogs->logInfo("Ascan [x,y] updated at: " + std::to_string(curpt.x) + " " + std::to_string(curpt.y)  + " position.");

}

std::shared_ptr<QImage> AscanFrame::CreateXZScan() {
    if (scandat.Amplitudes.empty()) {
        return nullptr;
    }

    uint64_t zsize = scandat.AmplitudeAxes[0].Quantity;
    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    cv::Mat image(xsize, zsize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();

    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t x = 0; x < xsize; ++x) {
            uint64_t index = z * (xsize * ysize) + curpt.y + x;

            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
                return nullptr;
            }
            
            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            image.at<cv::Vec3b>(x, z) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    cv::Mat rotatedImage;
    cv::rotate(image, rotatedImage, cv::ROTATE_90_COUNTERCLOCKWISE);

    int frameWidth = graphicsView->size().width();
    int frameHeight = graphicsView->size().height();

    double frameRatio = static_cast<double>(frameWidth) / static_cast<double>(frameHeight);
    double imageRatio = static_cast<double>(rotatedImage.cols) / static_cast<double>(rotatedImage.rows);

    cv::Mat scaledImage;

    if (frameRatio > imageRatio) {
        int newWidth = static_cast<int>(rotatedImage.rows * frameRatio);
        cv::resize(rotatedImage, scaledImage, cv::Size(newWidth, rotatedImage.rows), 0, 0, cv::INTER_LINEAR);
    }
    else {
        int newHeight = static_cast<int>(rotatedImage.cols / frameRatio);
        cv::resize(rotatedImage, scaledImage, cv::Size(rotatedImage.cols, newHeight), 0, 0, cv::INTER_LINEAR);
    }
    cv::GaussianBlur(scaledImage, scaledImage, cv::Size(5, 5), 0);

    //cv::GaussianBlur(rotatedImage, rotatedImage, cv::Size(5, 5), 0);


    std::shared_ptr<QImage> qImage = std::make_shared<QImage>(scaledImage.data, scaledImage.cols, scaledImage.rows, scaledImage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    return qImage;
}


void AscanFrame::CreateAScan()
{
    // ********** SETTINGS **********
    static QValueAxis* axisX = new QValueAxis();
    static QValueAxis* axisY = new QValueAxis();
    static QFont axisFont;

    // Pen for line series
    static QPen linePen(QColor(0, 102, 204));

    // Grid and tick settings

    // ********** PARAMETER VALIDATION **********
    if (scandat.Amplitudes.empty()) {
        if (sttlogs) {
            sttlogs->logWarning("No amplitude data available");
        }
        return;
    }

    // ********** INITIALIZATION & SETTINGS **********
    if (!chart || !lineSeries) {
        axisFont.setPointSize(8);
        linePen.setWidth(2);
        scene->clear();
        chart = new QChart();
        lineSeries = new QLineSeries();

        // Attach series and axes
        chart->addSeries(lineSeries);
        chart->legend()->hide();
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        lineSeries->attachAxis(axisX);
        lineSeries->attachAxis(axisY);
        lineSeries->setPen(linePen);
        lineSeries->setPointsVisible(false);

        // Axis settings
        axisX->setTitleText("Z Position");
        axisY->setTitleText("Amplitude");
        axisX->setTitleBrush(QBrush(QColor(Qt::darkCyan)));
		axisY->setTitleBrush(QBrush(QColor(Qt::darkCyan)));
        axisY->setLabelsColor(QColor(Qt::cyan));
        axisX->setLabelsFont(axisFont);
        axisY->setLabelsFont(axisFont);
        axisX->setLabelsColor(QColor(Qt::cyan));  // Set axis label color to cyan
        axisY->setLabelsColor(QColor(Qt::cyan));
        axisX->setGridLineColor(QColor(80,80,80)); // Set grid line color to gray
        axisY->setGridLineColor(QColor(80, 80, 80));
        axisY->setLabelFormat("%06.2f");
        axisX->setTickCount(10);  // Optional: Customize tick count
        axisY->setTickCount(10);

        // Chart settings
        chart->setBackgroundBrush(Qt::NoBrush);  // Transparent background
        chartView = new QChartView(chart);
        chartView->setStyleSheet("background: transparent");
        chartView->setAttribute(Qt::WA_TranslucentBackground);
        chartView->setRenderHint(QPainter::Antialiasing, true);
        chartView->setMinimumSize(800, 600);
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        chartView->setRubberBand(QChartView::RectangleRubberBand);

        scene->addWidget(chartView);
    }

    // ********** PARAMETER PROCESSING **********
    lineSeries->clear();
    uint64_t zsize = scandat.AmplitudeAxes[0].Quantity;
    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    QVector<QPointF> points;
    points.reserve(zsize);

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();

    for (uint64_t z = 0; z < zsize; ++z) {
        uint64_t index = z * (xsize * ysize) + curpt.y*xsize + curpt.x;

        if (index >= scandat.Amplitudes.size()) {
            sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
            return;
        }

        int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
        double percentAmplitude = samplingAmplitude / (32768 / 100.0);

        minY = std::min(minY, percentAmplitude);
        maxY = std::max(maxY, percentAmplitude);
        points.append(QPointF(z, percentAmplitude));

    }
    

    lineSeries->replace(points);
    // ********** DISPLAY **********
    // Auto-scale Y-axis based on data range
    axisY->setRange(-150, 150);

    // Auto-scale X-axis
    axisX->setRange(0, zsize);

    // Adjust aspect ratio based on the graphicsView's size
    int viewWidth = graphicsView->width();
    int viewHeight = graphicsView->height();
    double aspectRatio = static_cast<double>(viewWidth) / static_cast<double>(viewHeight);
    chartView->setFixedSize(viewWidth, viewHeight);

    graphicsView->update();
}
