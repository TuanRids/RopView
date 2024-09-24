#include "BscanFrame.h"

QWidget* BscanFrame::createFrame() {
    if (!graphicsView) {
        graphicsView = std::make_shared<ZoomableGraphicsView>();
    }
    if (!scene) {
        scene = std::make_shared<QGraphicsScene>();
    }
    graphicsView->setScene(scene.get());
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    MouseGetPosXY(graphicsView);

    // Main layout
    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->addWidget(graphicsView.get());
    frame->setLayout(layout);
    
    //// Container to hold both graphicsView and navigator
    //QWidget* containerWidget = new QWidget(frame);
    //containerWidget->setLayout(layout);
    //
    //// Navigator Mini View
    //navigatorView = new QGraphicsView(containerWidget);
    //navigatorView->setScene(scene.get());
    //navigatorView->setFixedSize(200, 150);
    //navigatorView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //navigatorView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //
    //// Absolute positioning for navigatorView in the top right corner
    //navigatorView->setGeometry(containerWidget->width() - navigatorView->width() - 10, 10, 200, 150);
    //navigatorView->raise();  // Make sure it's on top of the graphicsView
    //return containerWidget;

    return frame;

}



void BscanFrame::update() {   
    if (isPanningLocal) { return; }
    CreateYZScan();   
    render_graphic();
}


void BscanFrame::CreateYZScan() {
    if (scandat.Amplitudes.empty()) {
        return;
    }

    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    if (orgimage) { orgimage.reset();}
    if (scaledImage) { scaledImage.reset();  }

    scaledImage = std::make_unique<cv::Mat>();
    orgimage = std::make_unique<cv::Mat>(zsize, ysize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();
    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + curpt.x;

            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index) + " - " + std::to_string(scandat.Amplitudes.size()));
                return ;
            }

            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(z, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

}
std::pair<int, int> BscanFrame::calculateOriginalPos(int scaled_y, int scaled_z) {
    if (orgimage == nullptr || scaledImage == nullptr) {
        throw std::exception();
    }
    auto scale_y = static_cast<double>(orgimage->cols) / static_cast<double>(scaledImage->cols);
    auto scale_z = static_cast<double>(orgimage->rows) / static_cast<double>(scaledImage->rows);

    int original_y = static_cast<int>(scaled_y * scale_y);
    int original_z = static_cast<int>(scaled_z * scale_z);

    if (original_y < 0 || original_z < 0 || original_z >= zsize || original_y >= ysize) {
        throw std::exception();
    }
    return { original_y, original_z };
}


void BscanFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_y, int scaled_z) {
        try
        {
            auto [original_y, original_z] = calculateOriginalPos(scaled_y, scaled_z);

            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(curpt.x).arg(original_y).arg(original_z);
            QToolTip::showText(QCursor::pos(), tooltipText);

        }
        catch (const std::exception& e) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            auto [original_y, original_z] = calculateOriginalPos(scaled_y, scaled_z);

            curpt.y = original_y;
            curpt.z = original_z;
            isPanning = false;
            uiframe->refreshxyz();
        }
		catch (const std::exception& e) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseDragClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            auto [original_y, original_z] = calculateOriginalPos(scaled_y, scaled_z);

            curpt.y = original_y;
            curpt.z = original_z;
            isPanning = true;
            isPanningLocal = true;
            uiframe->refreshxyz();
        }
        catch (const std::exception& e) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseStopDragClicked, [=]() {
        try
        {
            isPanning = false;
            isPanningLocal = false;
            uiframe->refreshxyz();
        }
        catch (const std::exception& e) { (void)0; }
        });
}

void BscanFrame::render_graphic()
{

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
    // high resolution
    if (!isPanning || SettingsManager::getInstance()->getResolutionBscan())
    { cv::resize(*scaledImage, *scaledImage, cv::Size(scaledImage->cols * uiframe->get_resolution(), scaledImage->rows * uiframe->get_resolution()), 0, 0, cv::INTER_LANCZOS4); }
    else // low resolution
    { cv::resize(*scaledImage, *scaledImage, cv::Size(scaledImage->cols, scaledImage->rows), 0, 0, cv::INTER_NEAREST); }
    cv::GaussianBlur(*scaledImage, *scaledImage, cv::Size(5, 5), 0);

    auto qImage = std::make_shared<QImage>(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();

    QPixmap pixmap = QPixmap::fromImage(*qImage);
    scene->clear();
    scene->addPixmap(pixmap);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    graphicsView->update();
}

