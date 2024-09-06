#include "BscanFrame.h"
void BscanFrame::setUIFrame(nmainUI::UIFrame* ui) {
    uiframe = ui;
}

QWidget* BscanFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<ZoomableGraphicsView>();
    graphicsView->setScene(&*scene);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    // slider int for logical function
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 700);
    slider->setValue(0);
    QObject::connect(slider, &QSlider::valueChanged, [=](int value) mutable {
        x_level_ = static_cast<unsigned int>(value);
        uiframe->refreshxyz(x_level_, -1, -1);
        });

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->addWidget(graphicsView.get());
    layout->addWidget(slider);
    frame->setLayout(layout);

    return frame;
}

void BscanFrame::update() {
    if (!curpt.CheckIdx(x_level_, -1, -1)) {
        return;
    }
    std::shared_ptr<QImage> nimg = CreateYZScan();
    if (nimg && !nimg->isNull()) {
        QPixmap pixmap = QPixmap::fromImage(*nimg);
        scene->clear();
        scene->addPixmap(pixmap);
        graphicsView->update();
        sttlogs->logInfo("Bscan at: " + std::to_string(x_level_) + " position.");
    }
}
std::shared_ptr<QImage> BscanFrame::CreateYZScan() {
    if (scandat.Amplitudes.empty()) {
        return nullptr;
    }

    uint64_t zsize = scandat.AmplitudeAxes[0].Quantity;
    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    cv::Mat image(ysize, zsize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();

    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + x_level_;

            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
                return nullptr;
            }

            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            image.at<cv::Vec3b>(y, z) = cv::Vec3b(color.B, color.G, color.R);
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