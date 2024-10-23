#include "..\pch.h"
#include "ObserverMgr.h"
#include "SystemConfig/ConfigLocator.h"
AscanData nObserver::scandat;
nmainUI::statuslogs* nObserver::sttlogs = nullptr;
curpt3d nObserver::curpt{ 0,0,0 };
bool nObserver::isPanning = false;
deque<shared_ptr<IAscanCollection>> nObserver::nAscanCollection = deque<shared_ptr<IAscanCollection>>();


void nObserver::UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene,
    std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor)
{

    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(OrgImg->cols) / static_cast<double>(OrgImg->rows);

    auto newWidth = (frameRatio > imageRatio) ? static_cast<int>(OrgImg->rows * frameRatio) : OrgImg->cols;
    auto newHeight = (frameRatio > imageRatio) ? OrgImg->rows : static_cast<int>(OrgImg->cols / frameRatio);

    auto scaleFactor = (!isPanning || ConfigLocator::getInstance().settingconf.bhighResBscan) ? res : 1.0;
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

    QGraphicsLineItem* verticalLine = scene->addLine(-2, 0, -2, newHeight * scaleFactor + 4, gridx);


    QGraphicsLineItem* horizontalLine = scene->addLine(-4, newHeight * scaleFactor + 2, newWidth * scaleFactor, newHeight * scaleFactor + 2, gridy);


    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    graphicsView->update();
}

std::vector<Color> nObserver::CreateColorPalette()
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
