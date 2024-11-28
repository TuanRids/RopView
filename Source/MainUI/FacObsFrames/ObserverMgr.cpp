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
    QVector<VertexData> nObserver::vertice_sview = QVector<VertexData>(); 
    QVector<VertexData> nObserver::vertice_cview = QVector<VertexData>();
// Functions *******************************************************************************
nObserver::nObserver()
{
    if (!sttlogs)  sttlogs = &nmainUI::statuslogs::getinstance();
    if (!ArtScan) ArtScan = &UIArtScan::getInstance();
}



void nObserver::RealDatProcess() {
    static auto everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
    shared_ptr<IAscanCollection> RawAsanDat;
    {
        std::shared_lock<std::shared_mutex> lock(collectionMutex);
        if (nAscanCollection.empty() || !nAscanCollection.front()) {
            if (!nAscanCollection.empty()) {
                nAscanCollection.pop_front();
            }
            return;
        }
        RawAsanDat = std::move(nAscanCollection.front());
        nAscanCollection.pop_front();
    }
    int zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
    int ysize = static_cast<int>(RawAsanDat->GetCount());

    cv::Mat tempSViewBuf, tempCViewBuf, tempBViewBuf;
    QVector<QPointF> points(zsize);

    static uint16_t BCsize = 100;

    // Create temporary buffers to avoid race conditions
    tempSViewBuf = cv::Mat::zeros(zsize, ysize + (ConfigL->visualConfig->setPautMode == PautModeOmni::Linear ?
        sin(oms.OMS->BeamStartAngle * M_PI / 180) * zsize :
        2 * tan(oms.OMS->BeamStartAngle * M_PI / 180) * zsize), CV_8UC3);

    if (ArtScan->CViewBuf->empty() || ArtScan->BViewBuf->empty()) {
        tempCViewBuf = cv::Mat::zeros(ysize, BCsize, CV_8UC3);
        tempBViewBuf = cv::Mat::zeros(zsize, BCsize, CV_8UC3);
    }
    else {
        ArtScan->CViewBuf->copyTo(tempCViewBuf);
        ArtScan->BViewBuf->copyTo(tempBViewBuf);
    }

    int centerX = 0, centerY = 0;
    double maxRadius = static_cast<double>(tempSViewBuf.rows);
    double angle_default = -90;

#pragma omp parallel for
    for (int beamID = 0; beamID < ysize; ++beamID) {
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        double minAmplitude = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
        double maxAmplitudeSampling = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
        double maxAmplitudeUsable = RawAsanDat->GetAscan(beamID)->GetAmplitudeDataRange()->GetFloatingMax();
        double angle = oms.OMS->BeamStartAngle + angle_default + beamID;
        double radian = angle * CV_PI / 180.0;
        double maxAmplitudeCscan = 0;
        Color maxColor{};

        for (int z = 0; z < zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            if (ConfigL->visualConfig->setPautMode == PautModeOmni::Linear) {
                int offsetX = static_cast<int>(z * sin(oms.OMS->BeamStartAngle * M_PI / 180));
                int adjustedX = beamID + offsetX;
                if (adjustedX >= 0 && adjustedX < tempSViewBuf.cols && z >= 0 && z < tempSViewBuf.rows) {
                    auto& pixel = tempSViewBuf.at<cv::Vec3b>(z, adjustedX);
                    pixel = cv::Vec3b(color.B, color.G, color.R);
                }
            }
            else if (ConfigL->visualConfig->setPautMode == PautModeOmni::Sectorial) {
                double radius = maxRadius * (static_cast<double>(z) / zsize);
                int x = centerX + static_cast<int>(radius * cos(radian));
                int y = centerY - static_cast<int>(radius * sin(radian));
                if (x >= 0 && x < tempSViewBuf.cols && y >= 0 && y < tempSViewBuf.rows) {
                    auto& pixel = tempSViewBuf.at<cv::Vec3b>(y, x);
                    pixel = cv::Vec3b(color.B, color.G, color.R);
                }
            }

            if (percentAmplitude > maxAmplitudeCscan) {
                maxAmplitudeCscan = percentAmplitude;
                maxColor = color;
            }

            if (oms.OMS->beamCurrentID == beamID) {
                points[z] = QPointF(percentAmplitude, static_cast<double>(z));
                tempBViewBuf.at<cv::Vec3b>(z, 0) = cv::Vec3b(color.B, color.G, color.R);
            }
        }
        tempCViewBuf.at<cv::Vec3b>(beamID, 0) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
    }

    // Update ArtScan buffers
    {
        std::lock_guard<std::mutex> lock(ArtScanMutex);
        if (ArtScan->SViewBuf) { ArtScan->SViewBuf = nullptr; }
        ArtScan->SViewBuf = std::make_shared<cv::Mat>(tempSViewBuf);
        ArtScan->CViewBuf = std::make_shared<cv::Mat>(tempCViewBuf);
        ArtScan->BViewBuf = std::make_shared<cv::Mat>(tempBViewBuf);
        *ArtScan->AViewBuf = points;
    }

    RawAsanDat.reset();
}

void nObserver::RealDatProcessGPU()
{
    
    static auto everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
    shared_ptr<IAscanCollection> RawAsanDat;
    {
        std::unique_lock<std::shared_mutex> lock(collectionMutex);
        if (nAscanCollection.empty()) {
            return;
        }
        RawAsanDat = std::move(nAscanCollection.front()); 
        nAscanCollection.pop_front();
    }
    if (!RawAsanDat) { return;	}

    int zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
    int ysize = static_cast<int>(RawAsanDat->GetCount());

    QVector<VertexData> tempt_bView;
    QVector<VertexData> tempt_cView;
    tempt_bView.reserve(zsize * ysize);
    tempt_cView.reserve(ysize);

    double angle_default = -90;
#pragma omp parallel for
    for (int beamID = 0; beamID < ysize; ++beamID) {
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        double minAmplitude = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
        double maxAmplitudeSampling = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
        double maxAmplitudeUsable = RawAsanDat->GetAscan(beamID)->GetAmplitudeDataRange()->GetFloatingMax();
        double angle = oms.OMS->BeamStartAngle + angle_default + beamID;
        double radian = angle * CV_PI / 180.0;

        double maxAmplitudeCscan = 0;
        Color maxColor{};

        for (int z = 0; z < zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            VertexData vertex;
            vertex.color = QVector3D(static_cast<float>(color.R) / 255.0f,
                static_cast<float>(color.G) / 255.0f,
                static_cast<float>(color.B) / 255.0f);

            if (ConfigL->visualConfig->setPautMode == PautModeOmni::Linear) {
                int offsetX = static_cast<int>(z * sin(oms.OMS->BeamStartAngle * M_PI / 180));
                int adjustedX = beamID + offsetX;
                if (adjustedX >= 0 && adjustedX < zsize) {
                    float normalizedX = static_cast<float>(beamID) / 10 * 2.0f - 1.0f;
                    float normalizedZ = static_cast<float>(z) / zsize * 2.0f - 1.0f;

                    vertex.position = QVector2D(normalizedX, normalizedZ);
                    tempt_bView.append(vertex);
                }
            }
            else if (ConfigL->visualConfig->setPautMode == PautModeOmni::Sectorial) {
                // later
            }

            if (percentAmplitude > maxAmplitudeCscan) {
                maxAmplitudeCscan = percentAmplitude;
                maxColor = color;
            }
        }

        VertexData vertex_Cview;
        vertex_Cview.position = QVector2D(static_cast<float>(beamID) / ysize * 2.0f - 1.0f, 0.0f);
        vertex_Cview.color = QVector3D(static_cast<float>(maxColor.R) / 255.0f,
            static_cast<float>(maxColor.G) / 255.0f,
            static_cast<float>(maxColor.B) / 255.0f);
        tempt_cView.append(vertex_Cview);
    }

    if (!tempt_cView.isEmpty()) vertice_cview = tempt_cView;
    if (!tempt_bView.isEmpty()) vertice_sview = tempt_bView;
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


