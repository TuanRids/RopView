#include "SviewFrame.h"

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
    layout->addWidget(graphicsView.get());
    frame->setLayout(layout);
    return frame;
}

void SviewFrame::update() {   
    if (overlay) { overlay->clearEvery(); }
    if (scandat.Amplitudes.empty()) return;

    CreateYZview();   
    addPoints(true,-1,-1);
}


void SviewFrame::CreateYZview() {
    if (scandat.Amplitudes.empty()) return;

    static bool dataOnGPU = false;
    static cv::cuda::GpuMat gpuAmplitudes;
    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(zsize, ysize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    auto everyColors = CreateColorPalette();
    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + curpt.x;
            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index));
                return;
            }

            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(z, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }
    UpdateGraphic(orgimage, scaledImage, scene, graphicsView, uiframe->get_resolution(), Qt::green, Qt::red);
}



void SviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    /*for (auto item : scene->items()) {
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
    gpen.setCosmetic(true);*/

    double pixelY = (Cviewlink) ? static_cast<double>(curpt.y) * scaledImage->cols / ysize : static_cast<double>(x);
    double pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);

   /* auto* verticalLine = new QGraphicsLineItem(pixelY, 0, pixelY, scaledImage->rows);
    verticalLine->setPen(gpen);
    MarkLine->addToGroup(verticalLine);*/

    if (overlay) overlay->updatePoints(pixelY, pixelZ, Qt::red, Qt::color0);

    graphicsView->update();
}


std::pair<int, int> SviewFrame::calculateOriginalPos(int scaled_y, int scaled_z) {
    if (orgimage == nullptr || scaledImage == nullptr) throw std::exception();

    int original_y = static_cast<int>(scaled_y * static_cast<double>(orgimage->cols) / scaledImage->cols);
    int original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);

    if (original_y < 0 || original_z < 0 || original_z >= zsize || original_y >= ysize) throw std::exception();
    return { original_y, original_z };
}
void SviewFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    overlay = std::make_shared<XYOverlayGrid>(scene.get());
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_y, int scaled_z) {
        try
        {
            auto [original_y, original_z] = calculateOriginalPos(scaled_y, scaled_z);
            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(curpt.x).arg(original_y).arg(original_z);
            QToolTip::showText(QCursor::pos(), tooltipText);
            overlay->updateOverlay(scaled_y, scaled_z, scaledImage->cols, scaledImage->rows);
            graphicsView->update();

        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            std::tie(curpt.y, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);

            isPanning = false;
            addPoints(false, scaled_y, scaled_z);            
            uiframe->refreshxyz(this);            
        }
		catch (...) { (void)0; }
        });
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseDragClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            std::tie(curpt.y, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
            addPoints(false, scaled_y, scaled_z);
            isPanning = true;
            uiframe->refreshxyz(this);
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


