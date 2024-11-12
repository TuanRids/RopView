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
    static auto logger = spdlog::get("file_logger");
     
    static auto everyColors = CreateColorPalette(ConfigLocator::getInstance().sysParams->colorPalette);
    std::lock_guard<std::mutex> lock(collectionMutex);
    while (nAscanCollection.size() > 5) { nAscanCollection.pop_front(); }

    if (!nAscanCollection.front()) {
        logger->error("RawAsanDat is null. Buffer Size: {}", nAscanCollection.size());
        nAscanCollection.pop_front();
        return;
    }
    shared_ptr<IAscanCollection> RawAsanDat = std::move(nAscanCollection.front());

    int zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
    int ysize = static_cast<int>(RawAsanDat->GetCount());

    QVector<QPointF> points(zsize);
    cv::Mat cmat; cv::Mat bmat;
    // aview and sview would be reset as realtime
    {
        ArtScan->resetall();
        ArtScan->SViewBuf->create(zsize, ysize, CV_8UC3);
        ArtScan->AViewBuf->clear();
        if (ArtScan->CViewBuf->empty()) {
            ArtScan->CViewBuf->create(ysize, 500, CV_8UC3); ArtScan->CViewBuf->setTo(cv::Scalar(0, 0, 0));
            cmat.create(ysize, 500, CV_8UC3);
        }
        else {
            if (ArtScan->CViewBuf->cols > 2500) {
                ArtScan->CViewBuf = std::make_shared<cv::Mat>(ArtScan->CViewBuf->colRange(0, 499).clone());
            }
            cmat.create(ArtScan->CViewBuf->rows, ArtScan->CViewBuf->cols, ArtScan->CViewBuf->type());
            ArtScan->CViewBuf->copyTo(cmat);
        }
        if (ArtScan->BViewBuf->empty()) {
            ArtScan->BViewBuf->create(zsize, 500, CV_8UC3); ArtScan->BViewBuf->setTo(cv::Scalar(0, 0, 0));
            bmat.create(zsize, 500, CV_8UC3);
        }
        else {
            if (ArtScan->BViewBuf->cols > 2500) {
                ArtScan->BViewBuf = std::make_shared<cv::Mat>(ArtScan->BViewBuf->colRange(0, 499).clone());
            }
            bmat.create(ArtScan->BViewBuf->rows, ArtScan->BViewBuf->cols, ArtScan->BViewBuf->type());
            ArtScan->BViewBuf->copyTo(bmat);
        }
    }        
#pragma omp parallel for
    for (int beamID = 0; beamID < ysize; ++beamID) {
        double maxAmplitudeCscan = 0; Color maxColor{};
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();        
        double minAmplitude = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
        double maxAmplitudeSampling = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
        double maxAmplitudeUsable = RawAsanDat->GetAscan(beamID)->GetAmplitudeDataRange()->GetFloatingMax();

#pragma omp simd reduction(max:maxAmplitudeCscan)
        for (int z = 0; z < zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
            // double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / 32.768;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            ArtScan->SViewBuf->at<cv::Vec3b>(z, beamID) = cv::Vec3b(color.B, color.G, color.R);

            if ( oms.OMS->beamCurrentID == beamID ) {
                points[z] = QPointF(percentAmplitude, static_cast<double>(z)); 
                bmat.at<cv::Vec3b>(z, 0 ) = cv::Vec3b(color.B, color.G, color.R);
            }
            if (percentAmplitude > maxAmplitudeCscan) { maxColor = color; maxAmplitudeCscan = percentAmplitude; }
        }     
        cmat.at<cv::Vec3b>(beamID,0 ) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
    }

    *ArtScan->AViewBuf = points;
    ArtScan->CViewBuf->colRange(0, ArtScan->CViewBuf->cols - 1).copyTo(cmat.colRange(1, cmat.cols));
    ArtScan->CViewBuf = std::make_shared<cv::Mat>(cmat);
    ArtScan->BViewBuf->colRange(0, ArtScan->BViewBuf->cols - 1).copyTo(bmat.colRange(1, bmat.cols));
    ArtScan->BViewBuf = std::make_shared<cv::Mat>(bmat);
    RawAsanDat.reset();
}

std::vector<Color> nObserver::CreateColorPalette(int gainFactor = 0)
{
    std::vector<Color> colors;
    std::vector<Color> everyColors;
    colors.emplace_back(Color{ 255, 255, 255 });
    colors.emplace_back(Color{ 184, 212, 244 });
    colors.emplace_back(Color{ 113, 170, 233 });
    colors.emplace_back(Color{ 62, 105, 190 });
    colors.emplace_back(Color{ 14, 37, 143 });
    colors.emplace_back(Color{ 27, 72, 129 });
    colors.emplace_back(Color{ 59, 140, 127 });
    colors.emplace_back(Color{ 126, 187, 94 });
    colors.emplace_back(Color{ 211, 223, 45 });
    colors.emplace_back(Color{ 241, 211, 43 });
    colors.emplace_back(Color{ 222, 156, 80 });
    colors.emplace_back(Color{ 209, 121, 87 });
    colors.emplace_back(Color{ 205, 116, 49 });
    colors.emplace_back(Color{ 194, 98, 23 });
    colors.emplace_back(Color{ 167, 50, 26 });
    colors.emplace_back(Color{ 145, 12, 29 });

    size_t interpolationPoints = 10;
    double f = 1.0 / interpolationPoints;

    for (size_t colorIdx(0); colorIdx < colors.size() - 1; colorIdx++)
    {
        Color x = colors[colorIdx];
        Color y = colors[colorIdx + 1];

        for (size_t j(0); j < interpolationPoints; j++)
        {
            uint8_t r = static_cast<uint8_t>(x.R + (y.R - x.R) * (j * f));
            uint8_t g = static_cast<uint8_t>(x.G + (y.G - x.G) * (j * f));
            uint8_t b = static_cast<uint8_t>(x.B + (y.B - x.B) * (j * f));

            everyColors.emplace_back(Color{ r, g, b });
        }
    }

    return everyColors;
}


