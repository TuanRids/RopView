#include "BviewFrame.h"

QWidget* BviewFrame::createFrame() {
    if (!graphicsView) { graphicsView = std::make_shared<ZoomableGraphicsView>(); }
    if (!scene) { scene = std::make_shared<QGraphicsScene>(); }
    graphicsView->setScene(scene.get());
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    MouseGetPosXY(graphicsView);
    // Main layout
    auto* layout = new QVBoxLayout();
    auto* frame = new QWidget();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(graphicsView.get());
    frame->setLayout(layout);
    if (!overlay)   overlay = std::make_shared<XYOverlayGrid>(graphicsView.get(), scene.get());

    return frame;
}

void BviewFrame::updateOffLine() {
    if (isRealTime) isRealTime = false;
    if (scandat.Amplitudes.empty()) return;

    CreateArtFrame();    
    addPoints(true, -1, -1);
}

void BviewFrame::updateRealTime()
{

    return;
    if (!isRealTime) {
        scene->clear();
        isRealTime = true;
    }
    orgimage = std::make_shared<cv::Mat>(ArtScan->BViewBuf->clone());
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

    graphicsView->update();
}


void BviewFrame::CreateArtFrame()
{
    if (scandat.Amplitudes.empty()) return;

    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(zsize, xsize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    auto everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);

    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t x = 0; x < xsize; ++x) {
            uint64_t index = z * (xsize * ysize) + curpt.y * xsize + x;
            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index));
                return;
            }

            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(z, x) = cv::Vec3b(color.B, color.G, color.R);
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

void BviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    for (auto item : scene->items()) {
        if (item->data(0).toString() == "MarkLine") {
            scene->removeItem(item);
            delete item;
            break;
        }
    }

    auto* MarkLine = new QGraphicsItemGroup();
    MarkLine->setData(0, "MarkLine");
    scene->addItem(MarkLine);

    QPen gpen(Qt::blue);
    gpen.setWidth(3);
    gpen.setCosmetic(true);

    double pixelX = (Cviewlink) ? static_cast<double>(curpt.x) * scaledImage->cols / xsize : static_cast<double>(x);
    double pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);

    auto* verticalLine = new QGraphicsLineItem(pixelX, 0, pixelX, scaledImage->rows);
    verticalLine->setPen(gpen);
    MarkLine->addToGroup(verticalLine);

    if (overlay) overlay->updatePoints(pixelX, pixelZ, Qt::blue, Qt::color0);

    graphicsView->update();
}


std::pair<int, int> BviewFrame::calculateOriginalPos(int scaled_x, int scaled_z) {
    if (orgimage == nullptr || scaledImage == nullptr) throw std::exception();

    int original_x = static_cast<int>(scaled_x * static_cast<double>(orgimage->cols) / scaledImage->cols);
    int original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);

    if (original_x < 0 || original_z < 0 || original_z >= zsize || original_x >= xsize) throw std::exception();
    return { original_x, original_z };
}

void BviewFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_x, int scaled_z) {
        try
        {
            auto [original_x, original_z] = calculateOriginalPos(scaled_x, scaled_z);
            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(original_x).arg(curpt.y).arg(original_z);
            QToolTip::showText(QCursor::pos(), tooltipText);
            overlay->updateOverlay(scaled_x, scaled_z, scaledImage->cols, scaledImage->rows);
            graphicsView->update();

        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_x, int scaled_z) {
        try
        {
            std::tie(curpt.x, curpt.z) = calculateOriginalPos(scaled_x, scaled_z);

            isPanning = false;
            addPoints(false, scaled_x, scaled_z);

        }
        catch (...) { (void)0; }
        });
   
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseLeftView, [=]() {
        overlay->ClearLineGroup();
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::nKeyPressedEvent, [=]() {
        for (auto item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                graphicsView->fitInView(item->boundingRect(), Qt::KeepAspectRatio);
                break;
            }
        }

        });
}


