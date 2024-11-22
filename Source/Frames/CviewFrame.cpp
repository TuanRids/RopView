#include "../pch.h"
#include "CviewFrame.h"
//#include <opencv2/cudawarping.hpp> 

QWidget* CviewFrame::createFrame(){
    if (!scene) { scene = new QGraphicsScene; }
    if (!graphicsView) { graphicsView = new ZoomableGraphicsView; }
    graphicsView->setScene(scene);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    MouseGetPosXY();

    // Create layout and frame
    QVBoxLayout* layout = new QVBoxLayout();    
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(graphicsView);
        
    // Container
    QWidget* containerWidget = new QWidget();
    containerWidget->setLayout(layout);
    overlay = std::make_shared<XYOverlayGrid>(graphicsView, scene);

    return containerWidget;


}

void CviewFrame::update() {
    isRealTime = false;
    static int lastResolution = -1;
    if (scandat.Amplitudes.empty()) { return; }
    if (lastResolution != ConfigL->sysParams->resolution) {
        lastResolution = ConfigL->sysParams->resolution;
        CreateXYview();
    }
    // if Bscan and Cscan layer
    if (ConfigL->sysParams->resolution) {
        CreateXYview();
    }

    static bool lastIsCscanLayer = false;
    if (lastIsCscanLayer != ConfigL->sysParams->isCscanLayer) {
        lastIsCscanLayer = ConfigL->sysParams->isCscanLayer;
        CreateXYview();
    }
    addPoints(true,-1,-1);
}

void CviewFrame::updateRealTime()
{

    if (!isRealTime) {
        scene->clear();
        isRealTime = true;
    }
    orgimage = std::make_shared<cv::Mat>(ArtScan->CViewBuf->clone());
    if (!orgimage) return;

    scaledImage = std::make_unique<cv::Mat>();

    int originalWidth = orgimage->cols;
    int newWidth = graphicsView->size().width();

    int newHeight = graphicsView->size().height();

    cv::resize(*orgimage, *scaledImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);
    cv::flip(*scaledImage, *scaledImage, 1);
    auto qImage = std::make_shared<QImage>(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();

    QPixmap pixmap = QPixmap::fromImage(*qImage);
    for (auto item : scene->items()) {
        if (item->data(0).toString() == "artwork") {
            scene->removeItem(item);
            delete item;
            break;
        }
    }
    QGraphicsPixmapItem* artworkItem = scene->addPixmap(pixmap);
    artworkItem->setData(0, "artwork");
    /* TODO OPTIMIZE
    void CviewFrame::updateRealTime() {
    if (!isRealTime) {
        scene->clear();
        isRealTime = true;
    }

    orgimage = std::make_shared<cv::Mat>(ArtScan->CViewBuf->clone());
    if (!orgimage) return;

    scaledImage = std::make_unique<cv::Mat>();
    int originalWidth = orgimage->cols;
    int newWidth = graphicsView->size().width();
    int newHeight = graphicsView->size().height();

    cv::resize(*orgimage, *scaledImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);
    cv::Mat newCol = cv::Mat(scaledImage->rows, 1, scaledImage->type(), cv::Scalar(0, 0, 255));
    cv::hconcat(*scaledImage, newCol, *scaledImage);

    QImage qImage(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);
    qImage = qImage.rgbSwapped();

    QGraphicsPixmapItem* artworkItem = nullptr;
    for (auto item : scene->items()) {
        if (item->data(0).toString() == "artwork") {
            artworkItem = dynamic_cast<QGraphicsPixmapItem*>(item);
            break;
        }
    }

    if (!artworkItem) {
        QPixmap pixmap = QPixmap::fromImage(qImage);
        artworkItem = scene->addPixmap(pixmap);
        artworkItem->setData(0, "artwork");
    } else {
        QPixmap& pixmap = const_cast<QPixmap&>(artworkItem->pixmap());
        QPainter painter(&pixmap);
        painter.drawImage(0, 0, qImage);
        painter.end();
    }

    graphicsView->update();
}

    */
    graphicsView->update();
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

    std::vector<Color> everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
    uint64_t z_offset = curpt.z * (xsize * ysize);
    double percentAmplitude;

    // Processing amplitude data to assign colors
    for (uint64_t y = 0; y < ysize; ++y) {
        for (uint64_t x = 0; x < xsize; ++x) {
            if (!ConfigL->sysParams->isCscanLayer) {
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

    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(orgimage->cols) / static_cast<double>(orgimage->rows);

    auto newWidth = (frameRatio > imageRatio) ? static_cast<int>(orgimage->rows * frameRatio) : orgimage->cols;
    auto newHeight = (frameRatio > imageRatio) ? orgimage->rows : static_cast<int>(orgimage->cols / frameRatio);

    auto scaleFactor = (!isPanning || ConfigLocator::getInstance().settingconf->bhighResBscan) ? 1 : 1.0;
    cv::resize(*orgimage, *scaledImage, cv::Size(newWidth * scaleFactor, newHeight * scaleFactor), 0, 0, cv::INTER_LINEAR);

    cv::GaussianBlur(*scaledImage, *scaledImage, cv::Size(1, 1), 0);
    auto qImage = std::make_shared<QImage>(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();

    QPixmap pixmap = QPixmap::fromImage(*qImage);
    for (auto item : scene->items()) {
        if (item->data(0).toString() == "artwork") {
            scene->removeItem(item);
            delete item;
            break;
        }
    }
    QGraphicsPixmapItem* artworkItem = scene->addPixmap(pixmap);
    artworkItem->setData(0, "artwork");

    static bool first_flag = true; if (first_flag) graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio); first_flag = false;
    graphicsView->update();
}

void CviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    double pixelX= (Cviewlink) ? static_cast<double>(curpt.x) * scaledImage->cols / xsize : static_cast<double>(x);
    double pixelY = (Cviewlink) ? static_cast<double>(curpt.y) * scaledImage->rows / ysize : static_cast<double>(y);

    if (overlay) { overlay->updatePoints(pixelX, pixelY, Qt::blue, Qt::red); }
    graphicsView->update();
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

void CviewFrame::MouseGetPosXY()
{
    static int temX{ 0 }, temY{ 0 };
    QObject::connect(graphicsView, &ZoomableGraphicsView::mouseMoved, [=](int scaled_x, int scaled_y) {
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

    QObject::connect(graphicsView, &ZoomableGraphicsView::mouseClicked, [=](int scaled_x, int scaled_y) {
        try
        {
            temX = scaled_x; temY = scaled_y;
            std::tie(curpt.x, curpt.y) = calculateOriginalPos(scaled_x, scaled_y);
            isPanning = false;

            addPoints(false, scaled_x, scaled_y);
        }
        catch (...) { (void)0; }
        });
    

    QObject::connect(graphicsView, &ZoomableGraphicsView::mouseLeftView, [=]() {
        overlay->ClearLineGroup();
        });

    QObject::connect(graphicsView, &ZoomableGraphicsView::nKeyPressedEvent, [=]() {
        for (auto item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                graphicsView->fitInView(item->boundingRect(), Qt::KeepAspectRatio);
                break;
            }
        }

        });
}