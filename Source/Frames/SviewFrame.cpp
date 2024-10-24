#include "SviewFrame.h"
#include <omp.h>
#include "SystemConfig/ConfigLocator.h"


QWidget* SviewFrame::createFrame() {
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
    if (!overlay) { overlay = std::make_shared<XYOverlayGrid>(graphicsView.get(), scene.get()); }
    return frame;
}
void SviewFrame::update() {   
    isRealTime = false;
    if (scandat.Amplitudes.empty()) return;
    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(zsize, ysize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    auto everyColors = CreateColorPalette();
#pragma omp parallel for
    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + curpt.x;
            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index));
                return;
            }
            double percentAmplitude = std::abs(scandat.Amplitudes[index]) / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(z, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(orgimage->cols) / static_cast<double>(orgimage->rows);

    int newWidth = (frameRatio > imageRatio) ? static_cast<int>(orgimage->rows * frameRatio) : orgimage->cols;
    int newHeight = (frameRatio > imageRatio) ? orgimage->rows : static_cast<int>(orgimage->cols / frameRatio);

    int scaleFactor = (!isPanning || ConfigL.settingconf->bhighResBscan) ? ConfigL.sysParams->resolution : 1;
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

    auto artWork = new QGraphicsItemGroup();
    scene->addItem(artWork);
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
    pixmapItem->setData(0, "artwork");
    artWork->addToGroup(pixmapItem);

    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    graphicsView->update();
    addPoints(true,-1,-1);
}

void SviewFrame::updateRealTime()
{
    try {
        if (!isRealTime) { scene->clear(); isRealTime = true; }
        /*std::shared_ptr<IAscanCollection> Bdata;
        if (nAscanCollection.empty()) return;

        Bdata = nAscanCollection.front();
        int xsize = static_cast<int>(Bdata->GetCount());
        auto ascan = Bdata->GetAscan(0);  
        int ysize = static_cast<int>(ascan->GetSampleQuantity());


        orgimage = std::make_unique<cv::Mat>(ysize, xsize, CV_8UC3);
        scaledImage = std::make_unique<cv::Mat>();
#pragma omp parallel for
        auto everyColors = CreateColorPalette();
        for (int beamID = 0; beamID < xsize; ++beamID) {
            auto ascan = Bdata->GetAscan(beamID);  
            const int* ascanData = ascan->GetData();
            for (int y = 0; y < ysize; ++y) {
                double percentAmplitude = std::abs(ascanData[y]) / (32768 / 100.0);
                Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
                orgimage->at<cv::Vec3b>(y, beamID) = cv::Vec3b(color.B, color.G, color.R);
            }
        }*/
        orgimage.reset();
        orgimage = ArtScan->SViewBuf; 
        scaledImage = std::make_unique<cv::Mat>();
        cv::resize(*orgimage, *scaledImage, cv::Size(graphicsView->width(), graphicsView->height()), 0, 0, cv::INTER_NEAREST);
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
        auto artWork = new QGraphicsItemGroup();
        artWork->setZValue(-1);
        scene->addItem(artWork);
        QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
        pixmapItem->setData(0, "artwork");
        artWork->addToGroup(pixmapItem);
        graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        graphicsView->update();
    }
	catch (exception& e)
	{ std::cout << e.what() << std::endl; return; } 
}

void SviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    double pixelY, pixelZ;
    if (!isRealTime)
    {
        pixelY = (Cviewlink) ? static_cast<double>(curpt.y) * scaledImage->cols / ysize : static_cast<double>(x);
        pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);
    }
    else {
        pixelY = (Cviewlink) ? static_cast<double>(ConfigL.omconf->BeamPosition) * scaledImage->cols / ysize : static_cast<double>(x);
        pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);
    }

    if (overlay) overlay->updatePoints(pixelY, pixelZ, Qt::red, Qt::color0);
    graphicsView->update();
}


std::pair<int, int> SviewFrame::calculateOriginalPos(int scaled_y, int scaled_z) {
    if (scaledImage == nullptr) throw std::exception();
    int original_y; int original_z;
    if (!isRealTime)
    {
        original_y = static_cast<int>(scaled_y * static_cast<double>(orgimage->cols) / scaledImage->cols);
        original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);
        if (original_y < 0 || original_z < 0 || original_z >= zsize || original_y >= ysize) throw std::exception();
    }
    else
    {
		original_y = static_cast<int>(scaled_y * static_cast<double>(orgimage->cols) / scaledImage->cols);
		//int original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);
        original_z = scaled_z;
    }
    return { original_y, original_z };
}
void SviewFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_y, int scaled_z) {
        try
        {
            if (!isRealTime)
            {
                auto [original_y, original_z] = calculateOriginalPos(scaled_y, scaled_z);
                QToolTip::showText(QCursor::pos(), QString("X: %1\nY: %2\nZ: %3").arg(curpt.x).arg(original_y).arg(original_z));
                overlay->updateOverlay(scaled_y, scaled_z, scaledImage->cols, scaledImage->rows);
                graphicsView->update();
            }
            else
            {
                int newy;
                std::tie(newy, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
                if (!scaledImage) return;                
                QString tooltipText = QString("X: %1\nY: %2\nZ: %3")
                    .arg("...")
                    .arg(scaled_y != -1 ? QString::number(scaled_y) : "...")
                    .arg(scaled_z != -1 ? QString::number(scaled_z) : "...");

                QToolTip::showText(QCursor::pos(), tooltipText);
                overlay->updateOverlay(scaled_y, scaled_z, scaledImage->cols, scaledImage->rows);
            }
        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            if (!isRealTime)
            {
                std::tie(curpt.y, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
                isPanning = false;
                addPoints(false, scaled_y, scaled_z);

            }
            else
            {
                std::tie(ConfigL.omconf->BeamPosition, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
                isPanning = false;
                addPoints(false, scaled_y, scaled_z);
            }
        }
		catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseDragClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            if (!isRealTime)
            {
                std::tie(curpt.y, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
                addPoints(false, scaled_y, scaled_z);
                isPanning = true;
            }
        }
        catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseStopDragClicked, [=]() {
        try
        {
            if (!isRealTime)
            {                
                isPanning = false;
            }
        }
        catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseLeftView, [=]() {
        overlay->ClearLineGroup();
        });
}


