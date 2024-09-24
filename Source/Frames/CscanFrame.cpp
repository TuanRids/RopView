#include "..\pch.h"
#include "CscanFrame.h"

QWidget* CscanFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<ZoomableGraphicsView>();
    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();

    graphicsView->setScene(scene.get());
    MouseGetPosXY(graphicsView);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    
    layout->addWidget(graphicsView.get());
    frame->setLayout(layout);

    // Container to hold both graphicsView and navigator
    QWidget* containerWidget = new QWidget(frame);
    containerWidget->setLayout(layout);
    
    // Navigator Mini View
    navigatorView = new QGraphicsView(containerWidget);
    navigatorView->setScene(scene.get());
    navigatorView->setFixedSize(graphicsView->size().width()/10, graphicsView->size().height()/10);
    // transparent background
	navigatorView->setBackgroundBrush(Qt::transparent);
    navigatorView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    navigatorView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Absolute positioning for navigatorView in the top right corner
    navigatorView->setGeometry(20, 30, 200, 150);
    navigatorView->raise();  // Make sure it's on top of the graphicsView
    graphicsView->setNavigator(navigatorView);
    return containerWidget;

    return frame;
}

void CscanFrame::update() {
    static int lastResolution = -1;
    if (lastResolution != uiframe->get_resolution()) {
        lastResolution = uiframe->get_resolution();
        CreateXYScan();
        render_graphic();
    }
    // if Bscan and Cscan layer
    if (!isLocalPanning && uiframe->check_isCscanLayer()) {
        CreateXYScan();
        render_graphic();
    }
    
    static bool lastIsCscanLayer = false;
    if (lastIsCscanLayer != uiframe->check_isCscanLayer()) {
        lastIsCscanLayer = uiframe->check_isCscanLayer();
        CreateXYScan();
        render_graphic();
    }
}

void CscanFrame::CreateXYScan() {
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
}

void CscanFrame::render_graphic(){
    // Resize and adjust the image for the viewport
    int frameWidth = graphicsView->size().width();
    int frameHeight = graphicsView->size().height();
    double frameRatio = static_cast<double>(frameWidth) / static_cast<double>(frameHeight);
    double imageRatio = static_cast<double>(orgimage->cols) / static_cast<double>(orgimage->rows);

    if (frameRatio > imageRatio) {
        int newWidth = static_cast<int>(orgimage->rows * frameRatio);
        cv::resize(*orgimage, *scaledImage, cv::Size(newWidth, orgimage->rows), 0, 0, cv::INTER_LINEAR);
    }
    else {
        int newHeight = static_cast<int>(orgimage->cols / frameRatio);
        cv::resize(*orgimage, *scaledImage, cv::Size(orgimage->cols, newHeight), 0, 0, cv::INTER_LINEAR);
    }

    // Apply resolution and blur
    if (!isPanning)
    { cv::resize(*scaledImage, *scaledImage, cv::Size(scaledImage->cols * uiframe->get_resolution(), scaledImage->rows * uiframe->get_resolution()), 0, 0, cv::INTER_LANCZOS4); }

    cv::GaussianBlur(*scaledImage, *scaledImage, cv::Size(5, 5), 0);

    // Convert to QImage and display
    auto qImage = std::make_shared<QImage>(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(*qImage);
    scene->clear();
    scene->addPixmap(pixmap);

    // Fit the scene to the viewports   
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);    
    navigatorView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

    graphicsView->update();


}

std::pair<int, int> CscanFrame::calculateOriginalPos(int scaled_x, int scaled_y) {
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

void CscanFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_x, int scaled_y) {
        try
        {
            auto [original_x, original_y] = calculateOriginalPos(scaled_x, scaled_y);

            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(original_x).arg(original_y).arg(curpt.z);
            QToolTip::showText(QCursor::pos(), tooltipText);
        }
        catch (const std::exception& e) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_x, int scaled_y) {
        try
        {
            auto [original_x, original_y] = calculateOriginalPos(scaled_x, scaled_y);

            curpt.x = original_x;
            curpt.y = original_y;
            isPanning = false;
            uiframe->refreshxyz();
        }
        catch (const std::exception& e) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseDragClicked, [=](int scaled_x, int scaled_y) {
        try
        {
            auto [original_x, original_y] = calculateOriginalPos(scaled_x, scaled_y);

            curpt.x = original_x;
            curpt.y = original_y;
            isPanning = true;
            isLocalPanning = true;
            uiframe->refreshxyz();
        }
        catch (const std::exception& e) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseStopDragClicked, [=]() {
        try
        {
            isPanning = false;
			isLocalPanning = false;
            uiframe->refreshxyz();
        }
        catch (const std::exception& e) { (void)0; }
        });
}