#include "..\Source\Frames\nLogFrame.h"
#include "..\Source\Frames\CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"
#include "FactoryMgr.h"

AscanData nFrame::scandat;
nmainUI::statuslogs* nFrame::sttlogs = nullptr;
curpt3d nFrame::curpt { 0,0,0 };
bool nFrame::isPanning = false;
CircularBuffer<std::vector<int>> nFrame::sharedBuffer = CircularBuffer<std::vector<int>>(1000);

std::shared_ptr<nLogFrame> nFactoryFrame::crLogFrm() {
    return std::make_unique<nLogFrame>();
}

std::shared_ptr<CviewFrame> nFactoryFrame::crCviewFrm(nmainUI::UIFrame* uifmr) {
    auto res = std::make_unique<CviewFrame>();
    res->setUIFrame(uifmr);
    return res;     
}

std::shared_ptr<SviewFrame> nFactoryFrame::crSViewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<SviewFrame>();
    res->setUIFrame(uifmr);
    return res;

}

std::shared_ptr<AviewFrame> nFactoryFrame::crAviewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<AviewFrame>();
    res->setUIFrame(uifmr);
    return res;
}

std::shared_ptr<BviewFrame> nFactoryFrame::crBviewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<BviewFrame>();
    res->setUIFrame(uifmr);
    return res;
}

void nFrame::UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene, 
    std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor)
{

    //// Resize logic
    //double frameRatio = static_cast<double>(customPlot->width()) / static_cast<double>(customPlot->height());
    //double imageRatio = static_cast<double>(OrgImg->cols) / static_cast<double>(OrgImg->rows);
    //
    //auto newWidth = (frameRatio > imageRatio) ? static_cast<int>(OrgImg->rows * frameRatio) : OrgImg->cols;
    //auto newHeight = (frameRatio > imageRatio) ? OrgImg->rows : static_cast<int>(OrgImg->cols / frameRatio);
    //
    //auto scaleFactor = (!isPanning || SettingsManager::getInstance()->getSettings().bhighResBscan) ? res : 1.0;
    //cv::resize(*OrgImg, *Img, cv::Size(newWidth * scaleFactor, newHeight * scaleFactor), 0, 0, cv::INTER_LINEAR);
    //
    //// Convert the image to QImage and QPixmap
    //cv::GaussianBlur(*Img, *Img, cv::Size(1, 1), 0);
    //auto qImage = std::make_shared<QImage>(Img->data, Img->cols, Img->rows, Img->step, QImage::Format_RGB888);
    //*qImage = qImage->rgbSwapped();
    //
    //// Set background image in QCustomPlot
    //customPlot->axisRect()->setBackground(QPixmap::fromImage(*qImage));
    //
    //// Add grid and rulers
    //QCPGrid* grid = customPlot->xAxis->grid();
    //grid->setVisible(true);
    //grid->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    //
    ////customPlot->xAxis->setTickStep(10);  // Set the spacing for the rulers on X axis
    ////customPlot->yAxis->setTickStep(10);  // Set the spacing for the rulers on Y axis
    //
    //customPlot->rescaleAxes();
    //customPlot->replot();  // Redraw the plot with rulers and grid
    //return;

    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(OrgImg->cols) / static_cast<double>(OrgImg->rows);
    
    auto newWidth = (frameRatio > imageRatio) ? static_cast<int>(OrgImg->rows * frameRatio) : OrgImg->cols;
    auto newHeight = (frameRatio > imageRatio) ? OrgImg->rows : static_cast<int>(OrgImg->cols / frameRatio);
    
    auto scaleFactor = (!isPanning || SettingsManager::getInstance()->getSettings().bhighResBscan) ? res : 1.0;
    cv::resize(*OrgImg, *Img, cv::Size(newWidth * scaleFactor, newHeight * scaleFactor), 0, 0, cv::INTER_LINEAR);
    
    cv::GaussianBlur(*Img, *Img, cv::Size(1, 1), 0);
    auto qImage = std::make_shared<QImage>(Img->data, Img->cols, Img->rows, Img->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    
    QPixmap pixmap = QPixmap::fromImage(*qImage);
    scene->clear();
    QGraphicsPixmapItem* artworkItem = scene->addPixmap(pixmap);
    artworkItem->setData(0, "artwork");
    
    int gridSpacing = 20;
    QPen gridx(xcolor);
    QPen gridy(ycolor);
    gridx.setStyle(Qt::SolidLine); gridx.setWidth(0.003 * std::min(scene->height(), scene->width()));
    gridy.setStyle(Qt::SolidLine); gridy.setWidth(0.003 * std::min(scene->height(), scene->width()));

    QGraphicsLineItem* verticalLine = scene->addLine(-2, 0 , -2, newHeight * scaleFactor + 4, gridx);
    

    QGraphicsLineItem* horizontalLine = scene->addLine( - 4 , newHeight * scaleFactor + 2, newWidth*scaleFactor, newHeight * scaleFactor + 2, gridy);
    
    
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    graphicsView->update();
}

std::vector<Color> nFrame::CreateColorPalette()
{
    int gainFactor = 2;
    // Define a set of key colors that will be used as reference points for the gradient.
    std::vector<Color> colors;
    std::vector<Color> everyColors;

    // Key colors in the gradient, ranging from white to deep red.
    colors.emplace_back(Color{ 255, 255, 255 }); // white
    colors.emplace_back(Color{ 184, 212, 244 }); // light blue
    colors.emplace_back(Color{ 113, 170, 233 }); // blue
    colors.emplace_back(Color{ 62, 105, 190 });  // dark blue
    colors.emplace_back(Color{ 14, 37, 143 });   // dark purple
    colors.emplace_back(Color{ 27, 72, 129 });   // purple
    colors.emplace_back(Color{ 59, 140, 127 });  // green
    colors.emplace_back(Color{ 126, 187, 94 });  // light green
    colors.emplace_back(Color{ 211, 223, 45 });  // yellow
    colors.emplace_back(Color{ 241, 211, 43 });  // light yellow
    colors.emplace_back(Color{ 222, 156, 80 });  // orange
    colors.emplace_back(Color{ 209, 121, 87 });  // light orange
    colors.emplace_back(Color{ 205, 116, 49 });  // dark orange
    colors.emplace_back(Color{ 194, 98, 23 });   // brown
    colors.emplace_back(Color{ 167, 50, 26 });   // dark brown
    colors.emplace_back(Color{ 145, 12, 29 });   // red

    // Number of interpolation points between each pair of key colors.
    size_t interpolationPoints{ 7 };
    double f = 1.0 / interpolationPoints;   // Fractional step size for interpolation.

    // Interpolate between each pair of consecutive key colors.
    for (size_t colorIdx(0); colorIdx < colors.size() - 1; colorIdx++) {
        Color x = colors[colorIdx];         // Start color
        Color y = colors[colorIdx + 1];     // End color

        // Generate intermediate colors between x and y.
        for (size_t j(0); j < interpolationPoints; j++) {
            // Interpolate each RGB component separately.
            uint8_t r = static_cast<uint8_t>(x.R + (y.R - x.R) * (j * f));
            uint8_t g = static_cast<uint8_t>(x.G + (y.G - x.G) * (j * f));
            uint8_t b = static_cast<uint8_t>(x.B + (y.B - x.B) * (j * f));

            // Add the interpolated color to the resulting palette.
            everyColors.emplace_back(Color{ r, g, b });
        }
    }

    // Apply gainFactor to shift colors based on the intensity
    size_t numColors = everyColors.size();
    std::vector<Color> gainAdjustedColors(numColors);

    // Adjust the colors based on the gain factor.
    for (size_t i = 0; i < numColors; ++i) {
        // Calculate the shifted index based on gain.
        int shiftedIndex = static_cast<int>(i * gainFactor);
        if (shiftedIndex >= numColors) {
            shiftedIndex = static_cast<int>(numColors - 1); // Cap at the last color
        }
        gainAdjustedColors[i] = everyColors[shiftedIndex];
    }

    return gainAdjustedColors;

}
bool curpt3d::CheckIdx(int nx, int ny, int nz)
{
    if (nx != this->x && nx != -1) { this->x = nx;  return true; }
    if (ny != this->y && ny != -1) { this->y = ny; return true; }
    if (nz != this->z && nz != -1) { this->z = nz;  return true; }
    return false;
}