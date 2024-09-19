#include "..\pch.h"
#include "CscanFrame.h"


QWidget* CscanFrame::createFrame() {
    scene = std::make_shared<QGraphicsScene>();
    graphicsView = std::make_shared<ZoomableGraphicsView>();
    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();

    graphicsView->setScene(&*scene);
    MouseGetPosXY(graphicsView);
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    // slider int for logical function
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0,700);
    slider->setValue(1);
    QObject::connect(slider, &QSlider::valueChanged, [=](int value) mutable {
        z_level_ = static_cast<unsigned int>(value);  
        uiframe->refreshxyz();
        }); 
    
    layout->addWidget(slider);
    layout->addWidget(graphicsView.get());
    frame->setLayout(layout);

    return frame;
}

void CscanFrame::update() {
    if (!curpt.CheckIdx(-1, -1, z_level_)) {
        return;
    }    
    static bool first = true;
    if (first)
    {
        first = !first;
        CreateXYScan();
    }
}

void CscanFrame::CreateXYScan() {
    if (scandat.Amplitudes.empty()) {
        return;
    }

    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;
    zsize = scandat.AmplitudeAxes[0].Quantity; 

    cv::Mat orgimage(ysize, xsize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();

    for (uint64_t y = 0; y < ysize; ++y) {      // uint64_t x = 0; x < xsize; ++x
        for (uint64_t x = 0; x < xsize; ++x) {  // uint64_t y = 0; y < ysize; ++y
            
            int16_t maxAmplitude = 0;
            for (uint64_t z = 0; z < zsize; ++z) {
                uint64_t index = z * (xsize * ysize) + y * xsize + x;
                if (index >= scandat.Amplitudes.size()) {
                    sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
                    return;
                }
                int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
                // Combine multiple amplitude layer into one layer by using maximum amplitude method.
                // Another method is average method.
                if (samplingAmplitude > maxAmplitude) {
                    maxAmplitude = samplingAmplitude;
                }
            }
            double percentAmplitude = maxAmplitude / (32768 / 100.0);

            /*static uint64_t z_offset = z_level_ * (xsize * ysize);
            if (z_offset + y * xsize + x >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(z_offset + y * xsize + x) + " " + std::to_string(scandat.Amplitudes.size()));
                return;
            }
            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[z_offset + y * xsize + x]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);*/

            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage.at<cv::Vec3b>( y,x) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    int frameWidth = graphicsView->size().width();
    int frameHeight = graphicsView->size().height();

    double frameRatio = static_cast<double>(frameWidth) / static_cast<double>(frameHeight);
    double imageRatio = static_cast<double>(orgimage.cols) / static_cast<double>(orgimage.rows);
    cv::Mat scaledImage;

    if (frameRatio > imageRatio) {
        int newWidth = static_cast<int>(orgimage.rows * frameRatio);
        cv::resize(orgimage, scaledImage, cv::Size(newWidth, orgimage.rows), 0, 0, cv::INTER_LINEAR);
    }
    else {
        int newHeight = static_cast<int>(orgimage.cols / frameRatio);
        cv::resize(orgimage, scaledImage, cv::Size(orgimage.cols, newHeight), 0, 0, cv::INTER_LINEAR);
    }

    cv::GaussianBlur(scaledImage, scaledImage, cv::Size(5, 5), 0);

    // std::shared_ptr<QImage> qImage = std::make_shared<QImage>(scaledImage.data, scaledImage.cols, scaledImage.rows, scaledImage.step, QImage::Format_RGB888);
    std::shared_ptr<QImage> qImage = std::make_shared<QImage>(orgimage.data, orgimage.cols, orgimage.rows, orgimage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();

    QPixmap pixmap = QPixmap::fromImage(*qImage);
    scene->clear();
    scene->addPixmap(pixmap);
    graphicsView->update();
    return;
}

void CscanFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int x, int y) {
        if (curpt.x < 0 || curpt.y < 0 || x > xsize || y > ysize) {
            sttlogs->logCritical("Out of Range"); return;
        }
        curpt.x = x;
        curpt.y = y;
        uiframe->refreshxyz();
        });
}



//std::shared_ptr<QImage> CscanFrame::CreateXYScan() {
//    if (scandat.Amplitudes.empty()) {
//        return nullptr;
//    }
//    /*std::cout << "Amplitudes" << std::endl;
//    // print all data inside scandat.Amplitudes
//    for (size_t i = 0; i < scandat.Amplitudes.size(); ++i) {
//        std::cout << scandat.Amplitudes[i] << " ";
//    }*/
//
//    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
//    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;
//
//    cv::Mat image(xsize, ysize, CV_8UC3);
//    std::vector<Color> everyColors = CreateColorPalette();
//
//    uint64_t z_offset = z_level_ * (xsize * ysize);
//
//
//    for (uint64_t y = 0; y < ysize; ++y) {          // uint64_t x = 0; x < xsize; ++x
//        for (uint64_t x = 0; x < xsize; ++x) {      // uint64_t y = 0; y < ysize; ++y
//            // check if data is out of range: return
//            if (z_offset + y * xsize + x >= scandat.Amplitudes.size()) {
//                sttlogs->logWarning("Out of range data: " + std::to_string(z_offset + y * xsize + x) + " " + std::to_string(scandat.Amplitudes.size()));
//                return nullptr;
//            }
//            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[z_offset + y * xsize + x]);
//            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
//            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
//
//            image.at<cv::Vec3b>(x, y) = cv::Vec3b(color.B, color.G, color.R);
//        }
//    }
//
//    cv::Mat orgimage;
//    cv::rotate(image, orgimage, cv::ROTATE_90_COUNTERCLOCKWISE);
//
//
//    int frameWidth = graphicsView->size().width();
//    int frameHeight = graphicsView->size().height();
//
//    double frameRatio = static_cast<double>(frameWidth) / static_cast<double>(frameHeight);
//
//    double imageRatio = static_cast<double>(orgimage.cols) / static_cast<double>(orgimage.rows);
//    cv::Mat scaledImage;
//
//    if (frameRatio > imageRatio) {
//        int newWidth = static_cast<int>(orgimage.rows * frameRatio);
//        cv::resize(orgimage, scaledImage, cv::Size(newWidth, orgimage.rows), 0, 0, cv::INTER_LINEAR);
//    }
//    else {
//        int newHeight = static_cast<int>(image.cols / frameRatio);
//        cv::resize(orgimage, scaledImage, cv::Size(orgimage.cols, newHeight), 0, 0, cv::INTER_LINEAR);
//    }
//    cv::GaussianBlur(scaledImage, scaledImage, cv::Size(5, 5), 0);
//
//
//    std::shared_ptr<QImage> qImage = std::make_shared<QImage>(scaledImage.data, scaledImage.cols, scaledImage.rows, scaledImage.step, QImage::Format_RGB888);
//    *qImage = qImage->rgbSwapped();
//
//    std::shared_ptr<QImage> nsImage = std::make_shared<QImage>(orgimage.data, orgimage.cols, orgimage.rows, orgimage.step, QImage::Format_RGB888);
//    *nsImage = nsImage->rgbSwapped();
//
//    return nsImage;
//
//}