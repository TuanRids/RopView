#include "..\pch.h"
#include "CscanFrame.h"

void CscanFrame::setUIFrame(nmainUI::UIFrame* ui) {
    uiframe = ui;
}

QWidget* CscanFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<ZoomableGraphicsView>();
    graphicsView->setScene(&*scene);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    // slider int for logical function
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0,700);
    slider->setValue(0);
    QObject::connect(slider, &QSlider::valueChanged, [=](int value) mutable {
        z_level_ = static_cast<unsigned int>(value);  
        uiframe->refreshxyz(-1,-1,z_level_);
        });
    

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->addWidget(graphicsView.get());
    layout->addWidget(slider);
    frame->setLayout(layout);

    return frame;
}

void CscanFrame::update() {
    if (!curpt.CheckIdx(-1, -1, z_level_)) {
        return;
    }

    std::shared_ptr<QImage> nimg = CreateXYScan();
    if (nimg && !nimg->isNull()) {
        QPixmap pixmap = QPixmap::fromImage(*nimg);
        scene->clear();
        scene->addPixmap(pixmap);
        graphicsView->update();
        sttlogs->logInfo("Cscan at: " + std::to_string(z_level_) + " position.");
    }
}


std::shared_ptr<QImage> CscanFrame::CreateXYScan() {
    if (scandat.Amplitudes.empty()) {
        return nullptr;
    }
    std::cout << "Amplitudes" << std::endl;
    // print all data inside scandat.Amplitudes
    /*for (size_t i = 0; i < scandat.Amplitudes.size(); ++i) {
        std::cout << scandat.Amplitudes[i] << " ";
    }*/

    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    cv::Mat image(xsize, ysize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();

    uint64_t z_offset = z_level_ * (xsize * ysize);


    for (uint64_t x = 0; x < xsize; ++x) {
        for (uint64_t y = 0; y < ysize; ++y) {
            // check if data is out of range: return
            if (z_offset + y * xsize + x >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(z_offset + y * xsize + x) + " " + std::to_string(scandat.Amplitudes.size()));
                return nullptr;
            }
            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[z_offset + y * xsize + x]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            image.at<cv::Vec3b>(x, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    int frameWidth = graphicsView->size().width();
    int frameHeight = graphicsView->size().height();

    double frameRatio = static_cast<double>(frameWidth) / static_cast<double>(frameHeight);

    double imageRatio = static_cast<double>(image.cols) / static_cast<double>(image.rows);
    cv::Mat scaledImage;

    if (frameRatio > imageRatio) {
        int newWidth = static_cast<int>(image.rows * frameRatio);
        cv::resize(image, scaledImage, cv::Size(newWidth, image.rows), 0, 0, cv::INTER_LINEAR);
    }
    else {
        int newHeight = static_cast<int>(image.cols / frameRatio);
        cv::resize(image, scaledImage, cv::Size(image.cols, newHeight), 0, 0, cv::INTER_LINEAR);
    }
    cv::GaussianBlur(scaledImage, scaledImage, cv::Size(5, 5), 0);


    std::shared_ptr<QImage> qImage = std::make_shared<QImage>(scaledImage.data, scaledImage.cols, scaledImage.rows, scaledImage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();

    std::shared_ptr<QImage> nsImage = std::make_shared<QImage>(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    *nsImage = nsImage->rgbSwapped();

    return nsImage;

}