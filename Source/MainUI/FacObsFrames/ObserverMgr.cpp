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
     
    static auto everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
    std::lock_guard<std::mutex> lock(collectionMutex);
    while (nAscanCollection.size() > 5) { nAscanCollection.pop_front(); }

    if (!nAscanCollection.front()) {
        nAscanCollection.pop_front();
        return;
    }
    shared_ptr<IAscanCollection> RawAsanDat = std::move(nAscanCollection.front());

    int zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
    int ysize = static_cast<int>(RawAsanDat->GetCount());
    QVector<QPointF> points(zsize);
    cv::Mat cmat; cv::Mat bmat;
    // aview and sview would be reset as realtime

    // Calc for S-scan
        int centerX = /*ArtScan->SViewBuf->cols / 2*/0;
        int centerY = 0;
        double maxRadius = static_cast<double>(ArtScan->SViewBuf->rows);
        double angle_default = -90;
        //double angleMax = 30 ;
        //double angleStep = (angleMax - angleMin) / ysize;
    static uint16_t BCsize = 100;
    {
        ArtScan->resetall();
        if (ConfigL->visualConfig->setPautMode == PautModeOmni::Linear)
        {
            ArtScan->SViewBuf->create(zsize, ysize + sin(oms.OMS->BeamStartAngle * M_PI / 180) * zsize, CV_8UC3); ArtScan->SViewBuf->setTo(cv::Scalar(0, 0, 0));
        }
        else if (ConfigL->visualConfig->setPautMode == PautModeOmni::Sectorial)
        {
            ArtScan->SViewBuf->create(zsize, ysize + 2 * tan(oms.OMS->BeamStartAngle * M_PI / 180) * zsize, CV_8UC3); ArtScan->SViewBuf->setTo(cv::Scalar(0, 0, 0));
        }
        ArtScan->AViewBuf->clear();
        if (ArtScan->CViewBuf->empty()) {
            ArtScan->CViewBuf->create(ysize, BCsize, CV_8UC3); ArtScan->CViewBuf->setTo(cv::Scalar(0, 0, 0));
            cmat.create(ysize, BCsize, CV_8UC3);
        }
        else {
            if (ArtScan->CViewBuf->cols > 2500) {
                ArtScan->CViewBuf = std::make_shared<cv::Mat>(ArtScan->CViewBuf->colRange(0, 499).clone());
            }
            cmat.create(ArtScan->CViewBuf->rows, ArtScan->CViewBuf->cols, ArtScan->CViewBuf->type());
            ArtScan->CViewBuf->copyTo(cmat);
        }
        if (ArtScan->BViewBuf->empty()) {
            ArtScan->BViewBuf->create(zsize, BCsize, CV_8UC3); ArtScan->BViewBuf->setTo(cv::Scalar(0, 0, 0));
            bmat.create(zsize, BCsize, CV_8UC3);
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

        double angle = oms.OMS->BeamStartAngle + angle_default + beamID /** angleStep*/;
        double radian = angle * CV_PI / 180.0;
#pragma omp simd reduction(max:maxAmplitudeCscan)
        for (int z = 0; z < zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            if (ConfigL->visualConfig->setPautMode == PautModeOmni::Linear) {
                int offsetX = static_cast<int>(z * sin(oms.OMS->BeamStartAngle * M_PI / 180));
                int adjustedX = beamID + offsetX;
                if (adjustedX >= 0 && adjustedX < ArtScan->SViewBuf->cols && z >= 0 && z < ArtScan->SViewBuf->rows) {
                    uchar* row_ptr = ArtScan->SViewBuf->ptr<uchar>(z);
                    row_ptr[adjustedX * 3 + 0] = color.B;
                    row_ptr[adjustedX * 3 + 1] = color.G;
                    row_ptr[adjustedX * 3 + 2] = color.R;
                }
            }
            else if (ConfigL->visualConfig->setPautMode == PautModeOmni::Sectorial) {
                double radius = maxRadius * (static_cast<double>(z) / zsize);
                int x = centerX + static_cast<int>(radius * cos(radian));
                int y = centerY - static_cast<int>(radius * sin(radian));
                if (x >= 0 && x < ArtScan->SViewBuf->cols && y >= 0 && y < ArtScan->SViewBuf->rows) {
                    uchar* row_ptr = ArtScan->SViewBuf->ptr<uchar>(y);
                    row_ptr[x * 3 + 0] = color.B;
                    row_ptr[x * 3 + 1] = color.G;
                    row_ptr[x * 3 + 2] = color.R;
                }
                if (beamID+1 < ysize) 
                {
                    double nextAngle = oms.OMS->BeamStartAngle + angle_default + beamID + 1;
                    double nextRadian = nextAngle * CV_PI / 180.0;
                    int next_x = centerX + static_cast<int>(radius * cos(nextRadian));
                    int next_y = centerY - static_cast<int>(radius * sin(nextRadian));
                    if (next_x >= 0 && next_x < ArtScan->SViewBuf->cols && next_y >= 0 && next_y < ArtScan->SViewBuf->rows) {
                        cv::line(*ArtScan->SViewBuf, cv::Point(x, y), cv::Point(next_x, next_y), cv::Scalar(color.B, color.G, color.R), 1, cv::LINE_4);
                    }
                }
            }

            if (oms.OMS->beamCurrentID == beamID) {
                points[z] = QPointF(percentAmplitude, static_cast<double>(z));
                bmat.at<cv::Vec3b>(z, 0) = cv::Vec3b(color.B, color.G, color.R);
            }
            if (percentAmplitude > maxAmplitudeCscan) { maxColor = color; maxAmplitudeCscan = percentAmplitude; }
        }
        cmat.at<cv::Vec3b>(beamID, 0) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
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


