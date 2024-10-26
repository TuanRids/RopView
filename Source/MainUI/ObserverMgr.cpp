#include "..\pch.h"
#include "ObserverMgr.h"
#include "SystemConfig/ConfigLocator.h"
// Static variables *******************************************************************************
    AscanData nObserver::scandat;
    nmainUI::statuslogs* nObserver::sttlogs = nullptr;
    curpt3d nObserver::curpt{ 0,0,0 };
    bool nObserver::isPanning = false;
    deque<shared_ptr<IAscanCollection>> nObserver::nAscanCollection = deque<shared_ptr<IAscanCollection>>();
    size_t nObserver::_buffSize = 0;
    UIArtScan* nObserver::ArtScan = nullptr;

// Functions *******************************************************************************
nObserver::nObserver()
{
    if (!sttlogs)  sttlogs = &nmainUI::statuslogs::getinstance();
    if (!ArtScan) ArtScan = &UIArtScan::getInstance();
}

void nObserver::RealDatProcess()
{
    static auto everyColors = CreateColorPalette();
    std::lock_guard<std::mutex> lock(collectionMutex);
    _buffSize = 10 - nAscanCollection.size() > 0 ? 10 - nAscanCollection.size() : _buffSize;
    if (nAscanCollection.size() == 0) throw std::runtime_error("nAscanCollection is empty.");

    auto RawAsanDat = std::move(nAscanCollection.back());
    if (!RawAsanDat) throw std::runtime_error("nAscanCollection is empty.");
    while (nAscanCollection.size() > 10) nAscanCollection.pop_front();

    int zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
    int ysize = static_cast<int>(RawAsanDat->GetCount());

    // aview and sview would be reset as realtime
    ArtScan->resetall();
    ArtScan->SViewBuf->create(zsize, ysize, CV_8UC3);
    ArtScan->AViewBuf->clear();
    QVector<QPointF> points(zsize);
    cv::Mat newMat;
    if (ArtScan->CViewBuf->empty()) {
        ArtScan->CViewBuf->create(ysize, 500, CV_8UC3);
        newMat.create(ysize, 500, CV_8UC3);
    }
    else {
        if (ArtScan->CViewBuf->cols > 2500) {
            ArtScan->CViewBuf = std::make_shared<cv::Mat>(ArtScan->CViewBuf->colRange(0, 499).clone());
        }
        newMat.create(ArtScan->CViewBuf->rows, ArtScan->CViewBuf->cols, ArtScan->CViewBuf->type());
        ArtScan->CViewBuf->copyTo(newMat);
    }
        
#pragma omp parallel for
    for (int beamID = 0; beamID < ysize; ++beamID) {
        double maxAmplitude = 0; Color maxColor{};
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        for (int z = 0; z < zsize; ++z) {
            // Process Sscan Data
            double percentAmplitude = std::abs(ascanData[z]) / (32768 / 100.0);
            percentAmplitude = percentAmplitude < 2.5 ? percentAmplitude * 40 : percentAmplitude;

            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            ArtScan->SViewBuf->at<cv::Vec3b>(z, beamID) = cv::Vec3b(color.B, color.G, color.R);

            // Process Ascan Data
            if (ConfigLocator::getInstance().omconf->BeamPosition == beamID)
            {
                points[z] = QPointF(static_cast<double>(ascanData[z]), static_cast<double>(z));
            }
            // process Cscan collectdata
            if (percentAmplitude > maxAmplitude) {
                maxColor = color; maxAmplitude = percentAmplitude;
            }
        }
        // process Cscan        
        newMat.at<cv::Vec3b>(beamID,0 ) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
    }
    *ArtScan->AViewBuf = points;
    ArtScan->CViewBuf->colRange(0, ArtScan->CViewBuf->cols - 1).copyTo(newMat.colRange(1, newMat.cols));
    ArtScan->CViewBuf = std::make_shared<cv::Mat>(newMat);

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

