#include "BscanFrame.h"

QWidget* BscanFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<ZoomableGraphicsView>();  
    graphicsView->setScene(&*scene);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    MouseGetPosXY(graphicsView);
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 700);
    slider->setValue(0);
    QObject::connect(slider, &QSlider::valueChanged, [=](int value) mutable {
        x_level_ = static_cast<unsigned int>(value);
        uiframe->refreshxyz();
        });

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->addWidget(graphicsView.get());
    layout->addWidget(slider);
    frame->setLayout(layout);

    return frame;
}


void BscanFrame::update() {    
    std::shared_ptr<QImage> nimg = CreateYZScan();
    if (nimg && !nimg->isNull()) {
        QPixmap pixmap = QPixmap::fromImage(*nimg);
        scene->clear();
        scene->addPixmap(pixmap);
        graphicsView->update();
        sttlogs->logInfo("Bscan [x] at: " + std::to_string(curpt.x) + " position.");
    }
}


std::shared_ptr<QImage> BscanFrame::CreateYZScan() {
    if (scandat.Amplitudes.empty()) {
        return nullptr;
    }

    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    cv::Mat orgimage(zsize, ysize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();
    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + curpt.x;

            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index) + " - " + std::to_string(scandat.Amplitudes.size()));
                return nullptr;
            }

            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage.at<cv::Vec3b>(z, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }
    cv::Mat scaledImage;

    int frameWidth = graphicsView->size().width();
    int frameHeight = graphicsView->size().height();

    double frameRatio = static_cast<double>(frameWidth) / static_cast<double>(frameHeight);
    // scale images if necessary for exact position when clicking on it.
    /*if (frameRatio > 1.0) {
        auto delta_rows = orgimage.cols / (static_cast<int>(frameRatio) * orgimage.rows);
        cv::resize(orgimage, scaledImage, cv::Size(newWidth, orgimage.rows), 0, 0, cv::INTER_LINEAR);
    }*/
    double imageRatio = static_cast<double>(orgimage.cols) / static_cast<double>(orgimage.rows);


    if (frameRatio > imageRatio) {
        int newWidth = static_cast<int>(orgimage.rows * frameRatio);
        cv::resize(orgimage, scaledImage, cv::Size(newWidth, orgimage.rows), 0, 0, cv::INTER_LINEAR);
    }
    else {
        int newHeight = static_cast<int>(orgimage.cols / frameRatio);
        cv::resize(orgimage, scaledImage, cv::Size(orgimage.cols, newHeight), 0, 0, cv::INTER_LINEAR);
    }
    cv::GaussianBlur(orgimage, orgimage, cv::Size(5, 5), 0);
    cv::GaussianBlur(scaledImage, scaledImage, cv::Size(5, 5), 0);
    
    //cv::GaussianBlur(orgimage, orgimage, cv::Size(5, 5), 0);


    std::shared_ptr<QImage> qImage = std::make_shared<QImage>(orgimage.data, orgimage.cols, orgimage.rows, orgimage.step, QImage::Format_RGB888);
    // std::shared_ptr<QImage> qImage = std::make_shared<QImage>(scaledImage.data, scaledImage.cols, scaledImage.rows, scaledImage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    return qImage;
}

void BscanFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int y, int z) {
        if (curpt.z < 0 || curpt.y < 0 || z > zsize || y > ysize) {
            sttlogs->logCritical("Out of Range"); return;
        }
        curpt.y = y;
        curpt.z = z;
        uiframe->refreshxyz();
        });
}
