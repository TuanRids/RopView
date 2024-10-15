#include "..\pch.h"
#include "CviewFrame.h"

QWidget* CviewFrame::createFrame(){
    overlay = nullptr;
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<ZoomableGraphicsView>();
    graphicsView->setScene(scene.get());
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    MouseGetPosXY(graphicsView);

    // Create layout and frame
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(graphicsView.get());

    navigatorView = new QGraphicsView();
    navigatorView->setScene(scene.get());
    navigatorView->setFixedSize(graphicsView->size().width() / 10, graphicsView->size().height() / 10);
    navigatorView->setBackgroundBrush(Qt::transparent);
    navigatorView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    navigatorView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    navigatorView->setGeometry(20, 30, 200, 150); // Positioning navigator view
    navigatorView->raise();

    graphicsView->setNavigator(navigatorView);
        
    // Container
    QWidget* containerWidget = new QWidget();
    containerWidget->setLayout(layout);

    return containerWidget;


}

void CviewFrame::update() {
    if (scandat.Amplitudes.empty()) { return; }
    static int lastResolution = -1;
    if (overlay) { overlay->clearEvery(); }
    if (lastResolution != uiframe->get_resolution()) {
        lastResolution = uiframe->get_resolution();
        CreateXYview();
    }
    // if Bscan and Cscan layer
    if (uiframe->check_isCscanLayer()) {
        CreateXYview();
    }

    static bool lastIsCscanLayer = false;
    if (lastIsCscanLayer != uiframe->check_isCscanLayer()) {
        lastIsCscanLayer = uiframe->check_isCscanLayer();
        CreateXYview();
    }
    addPoints(true,-1,-1);
}

void CviewFrame::CreateXYview() {
    // Declaration and initialization
    if (scandat.Amplitudes.empty()) {
        return;
    }

    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(ysize, xsize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    std::vector<Color> everyColors = CreateColorPalette();
    uint64_t z_offset = curpt.z * (xsize * ysize);
    double percentAmplitude;

    // Processing amplitude data to assign colors
    for (uint64_t y = 0; y < ysize; ++y) {
        for (uint64_t x = 0; x < xsize; ++x) {
            if (!uiframe->check_isCscanLayer()) {
                int16_t maxAmplitude = 0;
                for (uint64_t z = 0; z < zsize; ++z) {
                    uint64_t index = z * (xsize * ysize) + y * xsize + x;
                    if (index >= scandat.Amplitudes.size()) {
                        sttlogs->logWarning("Out of range data: " + std::to_string(index) + " / " + std::to_string(scandat.Amplitudes.size()));
                        return;
                    }
                    int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
                    if (samplingAmplitude > maxAmplitude) {
                        maxAmplitude = samplingAmplitude;
                    }
                }
                percentAmplitude = maxAmplitude / (32768.0 / 100.0);
            }
            else {
                uint64_t index = z_offset + y * xsize + x;
                if (index >= scandat.Amplitudes.size()) {
                    sttlogs->logWarning("Out of range data: " + std::to_string(index) + " / " + std::to_string(scandat.Amplitudes.size()));
                    return;
                }
                int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
                percentAmplitude = samplingAmplitude / (32768.0 / 100.0);
            }

            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(y, x) = cv::Vec3b(color.B, color.G, color.R);
        }
    }
    UpdateGraphic(orgimage, scaledImage, scene, graphicsView, 1,Qt::blue, Qt::red);
    //UpdateGraphic(orgimage, scaledImage, scene, graphicsView, uiframe->get_resolution());

}

void CviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    double pixelX= (Cviewlink) ? static_cast<double>(curpt.x) * scaledImage->cols / xsize : static_cast<double>(x);
    double pixelY = (Cviewlink) ? static_cast<double>(curpt.y) * scaledImage->rows / ysize : static_cast<double>(y);

    if (overlay) {
        overlay->updatePoints(pixelX, pixelY, Qt::blue, Qt::red);
    }

    graphicsView->update();
    return;
}

std::pair<int, int> CviewFrame::calculateOriginalPos(int scaled_x, int scaled_y) {
    if (orgimage == nullptr || scaledImage == nullptr) {
        throw std::exception();
    }
    auto scale_x = static_cast<double>(orgimage->cols) / static_cast<double>(scaledImage->cols);
    auto scale_y = static_cast<double>(orgimage->rows) / static_cast<double>(scaledImage->rows);

    int original_x = static_cast<int>(scaled_x * scale_x);
    int original_y = static_cast<int>(scaled_y * scale_y);

    if (original_x < 0 || original_y < 0 || original_y >= ysize || original_x >= xsize) {
        throw std::exception();
    }
    return { original_x, original_y };
}

void CviewFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    static int temX{ 0 }, temY{ 0 };
    overlay = std::make_shared<XYOverlayGrid>(scene.get());
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_x, int scaled_y) {
        try
        {
            temX = scaled_x; temY = scaled_y;
            auto [original_x, original_y] = calculateOriginalPos(scaled_x, scaled_y);
            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(original_x).arg(original_y).arg(curpt.z);
            QToolTip::showText(QCursor::pos(), tooltipText);
            overlay->updateOverlay(scaled_x, scaled_y, scaledImage->cols, scaledImage->rows);
            graphicsView->update();

        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_x, int scaled_y) {
        try
        {
            temX = scaled_x; temY = scaled_y;
            std::tie(curpt.x, curpt.y) = calculateOriginalPos(scaled_x, scaled_y);
            isPanning = false;
            uiframe->refreshxyz(this);
            addPoints(false, scaled_x, scaled_y);
        }
        catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseDragClicked, [=](int scaled_x, int scaled_y) {
        try
        {
            temX = scaled_x; temY = scaled_y;
            std::tie(curpt.x, curpt.y) = calculateOriginalPos(scaled_x, scaled_y);
            isPanning = true;
            uiframe->refreshxyz(this);
            addPoints(false, scaled_x, scaled_y);
        }
        catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseStopDragClicked, [=]() {
        try
        {
            isPanning = false;
            uiframe->refreshxyz(this);
        }
        catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseLeftView, [=]() {
        overlay->ClearLineGroup();
        });
}