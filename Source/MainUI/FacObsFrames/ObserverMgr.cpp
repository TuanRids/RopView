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
    std::atomic<bool> nObserver::nIsGlTexture = true;
    std::mutex nObserver::ArtScanMutex = std::mutex();
    std::vector<std::vector<cv::Point>> nObserver::xySectorial = std::vector<std::vector<cv::Point>>();
// Functions *******************************************************************************
nObserver::nObserver()
{
    if (!sttlogs)  sttlogs = &nmainUI::statuslogs::getinstance();
    if (!ArtScan) ArtScan = &UIArtScan::getInstance();
}

void preXYsectorial(
    int ysize, int zsize, double beamStartAngle, double angleDefault,
    double maxRadius, int centerX, int centerY,
    std::vector<std::vector<cv::Point>>& xySectorial)
{
    static int lastYSize = -1;
    static int lastZSize = -1;

    if (ysize != lastYSize || zsize != lastZSize) {
        xySectorial.clear();
        xySectorial.resize(ysize, std::vector<cv::Point>(zsize));
        std::cout << "precalculate for XYsectorial with size: " << ysize << "x" << zsize << std::endl;

        for (int beamID = 0; beamID < ysize; ++beamID) {
            double angle = beamStartAngle + angleDefault + beamID;
            double radian = angle * CV_PI / 180.0;
            double cosValue = cos(radian);
            double sinValue = sin(radian);

            for (int z = 0; z < zsize; ++z) {
                double radius = maxRadius * (static_cast<double>(z) / zsize);
                int x = centerX + static_cast<int>(radius * cosValue);
                int y = centerY - static_cast<int>(radius * sinValue);
                xySectorial[beamID][z] = cv::Point(x, y);
            }
        }

        lastYSize = ysize;
        lastZSize = zsize;
    }
}


void nObserver::RealDatProcess() {
    shared_ptr<IAscanCollection> RawAsanDat;    
    static const uint16_t BCsize = 1000;
    static int zsize = -1; static int ysize = -1;
    static double angStart { oms.OMS->BeamStartAngle };
    static double angEnd { oms.OMS->beamNumber + oms.OMS->BeamStartAngle };
    static PautModeOmni pautmode = ConfigL->visualConfig->setPautMode;
    cv::Mat tempSViewBuf, tempCViewBuf, tempBViewBuf;
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

    // ****************************************************************************************
    // Update
    if (IOmConnect::isUpdate || zsize == -1) {
        zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
        ysize = static_cast<int>(RawAsanDat->GetCount());
        angStart = oms.OMS->BeamStartAngle ;
        angEnd = oms.OMS->beamNumber + oms.OMS->BeamStartAngle;
        pautmode = ConfigL->visualConfig->setPautMode;

        xySectorial.clear();
        xySectorial.resize(ysize, std::vector<cv::Point>(zsize));
        std::cout << "precalculate for XYsectorial with size: " << ysize << "x" << zsize << std::endl;

        int centerX = angStart < 0 ? (zsize * sin(((-angStart) * M_PI / 180))) : 0;
        for (int beamID = 0; beamID < ysize; ++beamID) {
            double angle = angStart + -90 + beamID;
            double radian = angle * CV_PI / 180.0;
            double cosValue = cos(radian);
            double sinValue = sin(radian);

            for (int z = 0; z < zsize; ++z) {
                double radius = zsize * (static_cast<double>(z) / zsize);
                int x = centerX + static_cast<int>(radius * cosValue);
                int y = 0 - static_cast<int>(radius * sinValue);
                xySectorial[beamID][z] = cv::Point(x, y);
            }
        }
        IOmConnect::isUpdate = false;
        return;
    }

    //*****************************************************************************************
    // Process
    int sview_x = pautmode == PautModeOmni::Linear ?
         sin(angStart * M_PI / 180) * zsize : angStart < 0 ?
         (sin(((-angStart) * M_PI / 180)) + sin((angEnd) * M_PI / 180)) * zsize :
         sin((angEnd) * M_PI / 180) * zsize;

    tempSViewBuf = cv::Mat::zeros( zsize, sview_x, CV_8UC3);
    tempSViewBuf.setTo(cv::Vec3b(255, 0, 0));

    if (!ArtScan->CViewBuf || ArtScan->CViewBuf->empty() || ArtScan->BViewBuf->empty()) {
        tempCViewBuf = cv::Mat::zeros(ysize, BCsize, CV_8UC3);
        tempBViewBuf = cv::Mat::zeros(zsize, BCsize, CV_8UC3);
    }
    else {
        if (ArtScan->BViewBuf->rows < zsize || ArtScan->BViewBuf->cols < ysize) {
            tempCViewBuf = cv::Mat::zeros(ysize, BCsize, CV_8UC3);
            tempBViewBuf = cv::Mat::zeros(zsize, BCsize, CV_8UC3);
        }
        else
        {
            ArtScan->CViewBuf->copyTo(tempCViewBuf);
            ArtScan->BViewBuf->copyTo(tempBViewBuf);
        }
    }
    QVector<glm::vec2> points(zsize);

#pragma omp parallel for
    for (int beamID = 0; beamID < ysize; ++beamID) {
        double maxAmplitudeCscan = 0;
        Color maxColor{};

        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        double minAmplitude = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
        double maxAmplitudeSampling = RawAsanDat->GetAscan(beamID)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
        double maxAmplitudeUsable = RawAsanDat->GetAscan(beamID)->GetAmplitudeDataRange()->GetFloatingMax();

        for (int z = 0; z < zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            if (pautmode == PautModeOmni::Linear) {
                int offsetX = static_cast<int>(z * sin(angStart * M_PI / 180));
                int adjustedX = beamID + offsetX;
                if (adjustedX >= 0 && adjustedX < tempSViewBuf.cols && z >= 0 && z < tempSViewBuf.rows) {
                    auto& pixel = tempSViewBuf.at<cv::Vec3b>(z, adjustedX);
                    pixel = cv::Vec3b(color.B, color.G, color.R);
                }
            }
            
            else if (pautmode == PautModeOmni::Sectorial) {
                const cv::Point& point = xySectorial[beamID][z];
                int x = point.x;
                int y = point.y;

                if (x >= 0 && x < tempSViewBuf.cols && y >= 0 && y < tempSViewBuf.rows) {
                    uchar* row_ptr = tempSViewBuf.ptr<uchar>(y);
                    row_ptr[x * 3 + 0] = color.B;
                    row_ptr[x * 3 + 1] = color.G;
                    row_ptr[x * 3 + 2] = color.R;
                }
                if (beamID + 1 < ysize)
                {
                    const cv::Point& nextPoint = xySectorial[beamID + 1][z];
                    cv::line(tempSViewBuf, point, nextPoint, cv::Scalar(color.B, color.G, color.R), 1, cv::LINE_4);
                }
            }
            
            if (percentAmplitude > maxAmplitudeCscan) {
                maxAmplitudeCscan = percentAmplitude;
                maxColor = color;
            }

            if (oms.OMS->beamCurrentID == beamID) {
                points[z] = glm::vec2((percentAmplitude / 50.0) - 1.0, -1.0 + 2.0 * static_cast<double>(z) / static_cast<double>(zsize - 1));
                tempBViewBuf.at<cv::Vec3b>(z, 0) = cv::Vec3b(color.B, color.G, color.R);
            }
        }

        tempCViewBuf.at<cv::Vec3b>(beamID, 0) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
    }
    {
        std::lock_guard<std::mutex> lock(ArtScanMutex);
        if (ArtScan->SViewBuf) { ArtScan->SViewBuf = nullptr; }
        ArtScan->SViewBuf = std::make_shared<cv::Mat>(tempSViewBuf);
        ArtScan->CViewBuf = std::make_shared<cv::Mat>(tempCViewBuf);
        ArtScan->CViewBuf->colRange(0, ArtScan->CViewBuf->cols - 1).copyTo(tempCViewBuf.colRange(1, tempCViewBuf.cols));

        ArtScan->BViewBuf = std::make_shared<cv::Mat>(tempBViewBuf);
        ArtScan->BViewBuf->colRange(0, ArtScan->BViewBuf->cols - 1).copyTo(tempBViewBuf.colRange(1, tempBViewBuf.cols));
        *ArtScan->AViewBuf = points;

        static int lastYSize = -1;
        if (lastYSize != ysize) {
            size_t totalPixelsSView = ArtScan->SViewBuf->rows * ArtScan->SViewBuf->cols;
            size_t totalPixelsCView = ArtScan->CViewBuf->rows * ArtScan->CViewBuf->cols;
            size_t totalPixelsBView = ArtScan->BViewBuf->rows * ArtScan->BViewBuf->cols;
            size_t totalPixelsAView = ArtScan->AViewBuf->size();
            size_t totalPixels = totalPixelsSView + totalPixelsCView + totalPixelsBView + totalPixelsAView;
            std::stringstream ss;
            ss << "Total pixel count:\n";
            ss << "   SViewBuf: " << totalPixelsSView << " pixels\n";
            ss << "   CViewBuf: " << totalPixelsCView << " pixels\n";
            ss << "   BViewBuf: " << totalPixelsBView << " pixels\n";
            ss << "   AViewBuf: " << totalPixelsAView << " pixels\n";
            ss << "   Total: " << totalPixels << " pixels\n";
            sttlogs->logInfo(ss.str());
            lastYSize = ysize;
        }
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
                    float normalizedX = static_cast<float>(beamID)  / zsize * 2.0f - 1.0f;
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


