#include "..\pch.h"
#include "ObserverMgr.h"
#include "SystemConfig/ConfigLocator.h"
// Static variables *******************************************************************************
    AscanData nObserver::scandat;
    nmainUI::statuslogs* nObserver::sttlogs = nullptr;
    int nObserver::curpt[3] = { 0,0,0 };
    bool nObserver::isPanning = false;
    deque<shared_ptr<IAscanCollection>> nObserver::nAscanCollection = deque<shared_ptr<IAscanCollection>>();
    UIArtScan* nObserver::ArtScan = nullptr;
    QVector<VertexData> nObserver::vertice_sview = QVector<VertexData>(); 
    QVector<VertexData> nObserver::vertice_cview = QVector<VertexData>();
    std::atomic<bool> nObserver::nIsGlTexture = true;
    std::mutex nObserver::ArtScanMutex = std::mutex();
    std::vector<std::vector<cv::Point>> nObserver::xySectorial = std::vector<std::vector<cv::Point>>();

    //=======================================================================================
    //temporary
    GLuint nObserver::sectorialBuffer = 0; //sectorialBuffer

// Init Funcs *******************************************************************************
std::vector<Color> CreateColorPalette(int gainFactor = 0)
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
nObserver::nObserver()
{
    if (!sttlogs)  sttlogs = &nmainUI::statuslogs::getinstance();
    if (!ArtScan) ArtScan = &UIArtScan::getInstance();
    everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
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

// Process Func *******************************************************************************
void nObserver::RealDatProcess() {
    shared_ptr<IAscanCollection> RawAsanDat;    
    static const uint16_t BCsize = 1000;
    static int zsize = -1; static int ysize = -1;
    static double angStart { oms.OMS->BeamStartAngle };
    static double angEnd { oms.OMS->beamNumber + oms.OMS->BeamStartAngle };
    static PautModeOmni pautmode = ConfigL->visualConfig->setPautMode;
    static int sview_x = -1;
    static double minAmplitude = 1000.0f;
    static double maxAmplitudeSampling = -1.0f;
    static double maxAmplitudeUsable = -1.0f;

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
        {
            std::shared_lock<std::shared_mutex> lock(collectionMutex);
            if (nAscanCollection.size() > 0)
            {
                while (nAscanCollection.size() > 0) { nAscanCollection.pop_front(); }
                zsize == -1; return;
            }
        }

        // update all parameters
        zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
        RawAsanDat->GetAscan(0)->GetAmplitudeDataRange()->GetUnit();
        ysize = static_cast<int>(RawAsanDat->GetCount());
        angStart = oms.OMS->BeamStartAngle ;
        angEnd = oms.OMS->beamNumber + oms.OMS->BeamStartAngle;
        pautmode = ConfigL->visualConfig->setPautMode;

        // reset and clear all render buffer
        ArtScan->resetAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        xySectorial.clear();
        xySectorial.resize(ysize, std::vector<cv::Point>(zsize));

        // recalculate for BeamSet
        CoutLogger()->debug("precalculate for XYsectorial with size: " + std::to_string(ysize) + "x" + std::to_string(zsize));
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
        sview_x = (pautmode == PautModeOmni::Linear) ?
            ysize + std::abs(sin(angStart * M_PI / 180) * zsize) :
            angStart < 0 ?
                (sin(((-angStart) * M_PI / 180)) + sin((angEnd)*M_PI / 180)) * zsize :
                sin((angEnd)*M_PI / 180) * zsize;

        minAmplitude = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
        maxAmplitudeSampling = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
        maxAmplitudeUsable = RawAsanDat->GetAscan(0)->GetAmplitudeDataRange()->GetFloatingMax();

        sttlogs->logInfo("Update Amplitude Range: \nminAmplitude: " + std::to_string(minAmplitude) + " maxAmplitudeSampling: " + std::to_string(maxAmplitudeSampling) +
            " maxAmplitudeUsable: " + std::to_string(maxAmplitudeUsable));

        IOmConnect::isUpdate = false;
        return;
    }

    //*****************************************************************************************
    // Process
    tempSViewBuf = cv::Mat::zeros(zsize, sview_x, CV_8UC3);
    tempSViewBuf.setTo(cv::Scalar(0, 0, 0, 0));


    if (!ArtScan->CViewBuf || ArtScan->CViewBuf->empty() || !ArtScan->BViewBuf || ArtScan->BViewBuf->empty()) {
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
    std::vector<glm::vec2> points(zsize);
    //check if zsize > SampleQuantity: return for update
    if (std::abs(zsize - static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity())) > 2) {
        CoutLogger()->critical("Process: Zsize > Sample Quantity. Need to Update");
        zsize = -1; return;
    }

#pragma omp parallel for
    for (int beamID = 0; beamID < ysize; ++beamID) {
        double maxAmplitudeCscan = 0;
        Color maxColor{};
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        for (int z = 0; z < zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            int adjustedX{ -1 }; 
            if (pautmode == PautModeOmni::Linear) {
                int offsetX = std::abs(static_cast<int>(z * sin(angStart * M_PI / 180)));
                adjustedX = beamID + offsetX;
                if (adjustedX >= 0 && adjustedX < tempSViewBuf.cols && z >= 0 && z < tempSViewBuf.rows) {
                    auto& pixel = tempSViewBuf.at<cv::Vec3b>(z, adjustedX);
                    pixel = cv::Vec3b(color.B, color.G, color.R);
                }                
            }
            
            else if (pautmode == PautModeOmni::Sectorial || pautmode == PautModeOmni::Compound) {
                const cv::Point& point = xySectorial[beamID][z];
                adjustedX = point.x;
                int y = point.y;

                if (adjustedX >= 0 && adjustedX < tempSViewBuf.cols && y >= 0 && y < tempSViewBuf.rows) {
                    uchar* row_ptr = tempSViewBuf.ptr<uchar>(y);
                    row_ptr[adjustedX * 3 + 0] = color.B;
                    row_ptr[adjustedX * 3 + 1] = color.G;
                    row_ptr[adjustedX * 3 + 2] = color.R;
                }
                if (beamID + 1 < ysize)
                {
                    const cv::Point& nextPoint = xySectorial[beamID + 1][z];
                    // currently use too much time to process, low performance TODO optimize for highest resolution
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
                if ( z == zsize - 1) //TODO: bring to GPU; need to convert cv pos into gltexture2D quad
                {
                    if (pautmode == PautModeOmni::Linear)
                        cv::line(tempSViewBuf, cv::Point(beamID, 0), cv::Point(adjustedX, tempSViewBuf.rows), cv::Scalar(10, 10, 160, 256), 1, cv::LINE_4);
                    else if (pautmode == PautModeOmni::Sectorial || pautmode == PautModeOmni::Compound)
                        cv::line(tempSViewBuf, xySectorial[0][0], xySectorial[beamID][zsize - 1], cv::Scalar(10, 10, 160, 256), 1, cv::LINE_4);
                }
            }
        }
        if (beamID >= tempCViewBuf.rows) { zsize = -1; return; }
        tempCViewBuf.at<cv::Vec3b>(beamID, 0) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
    }
    
    cv::GaussianBlur(tempSViewBuf, tempSViewBuf, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    if (angStart < 0 && pautmode == PautModeOmni::Linear) cv::flip(tempSViewBuf, tempSViewBuf, 1);
    {
        std::lock_guard<std::mutex> lock(ArtScanMutex);
        if (ArtScan->SViewBuf) { ArtScan->SViewBuf = nullptr; }
        ArtScan->SViewBuf = std::make_shared<cv::Mat>(tempSViewBuf);
        ArtScan->CViewBuf = std::make_shared<cv::Mat>(tempCViewBuf);
        ArtScan->CViewBuf->colRange(0, ArtScan->CViewBuf->cols - 1).copyTo(tempCViewBuf.colRange(1, tempCViewBuf.cols));

        ArtScan->BViewBuf = std::make_shared<cv::Mat>(tempBViewBuf);
        ArtScan->BViewBuf->colRange(0, ArtScan->BViewBuf->cols - 1).copyTo(tempBViewBuf.colRange(1, tempBViewBuf.cols));
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
void nObserver::processOnGPU()
{ 
    static std::shared_ptr<upFrame> upframe = std::make_shared<upFrame>();
    upframe->processOnGPU();
}
void upFrame::processOnGPU()
{
    shared_ptr<IAscanCollection> RawAsanDat;

    static const uint16_t BCsize = 1000;
    static int zsize = -1; static int ysize = -1;
    static double angStart{ oms.OMS->BeamStartAngle };
    static double angEnd{ oms.OMS->beamNumber + oms.OMS->BeamStartAngle };
    static PautModeOmni pautmode = ConfigL->visualConfig->setPautMode;
    static int sview_x = -1;

    static double minAmplitude = 1000.0f;
    static double maxAmplitudeSampling = -1.0f;
    static double maxAmplitudeUsable = -1.0f;

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
        angStart = oms.OMS->BeamStartAngle;
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
        sview_x = (pautmode == PautModeOmni::Linear) ?
            ysize + std::abs(sin(angStart * M_PI / 180) * zsize) :
            angStart < 0 ?
            (sin(((-angStart) * M_PI / 180)) + sin((angEnd)*M_PI / 180)) * zsize :
            sin((angEnd)*M_PI / 180) * zsize;

        minAmplitude = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
        maxAmplitudeSampling = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
        maxAmplitudeUsable = RawAsanDat->GetAscan(0)->GetAmplitudeDataRange()->GetFloatingMax();

        sttlogs->logInfo("Update Amplitude Range: \nminAmplitude: " + std::to_string(minAmplitude) + " maxAmplitudeSampling: " + std::to_string(maxAmplitudeSampling) +
            " maxAmplitudeUsable: " + std::to_string(maxAmplitudeUsable));

        IOmConnect::isUpdate = false;
        return;
    }
    // ****************************************************************************************
    // Compute shader
    static std::unique_ptr<QOpenGLShaderProgram> computeShader = nullptr;
    QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();
    if (!computeShader)
    {
        computeShader = std::make_unique<QOpenGLShaderProgram>(QOpenGLContext::currentContext());
        computeShader->addShaderFromSourceCode(QOpenGLShader::Compute,
            R"(
            #version 430 core

            layout(local_size_x = 16, local_size_y = 16) in; 
            layout(rgba8, binding = 0) uniform image2D img_output;

            // Uniforms
            uniform int zsize;            
            uniform int ysize;          
            uniform vec3 u_amplitudeParams; // minAmplitude, maxAmplitudeSampling, maxAmplitudeUsable
            layout(std430, binding = 1) buffer AscanDataBuffer {
                int ascanData[];// 1D: beamID * zsize + z
            };

            // Sectorial mapping
            layout(std430, binding = 2) buffer SectorialMapping {
                ivec2 sectorialPoints[]; // raster (beamID * zsize + z)
            };

            // Color palette
            layout(std430, binding = 3) buffer ColorPalette {
                vec3 colors[256]; 
            };

            void main() {
                ivec2 gid = ivec2(gl_GlobalInvocationID.xy); // Global thread ID
                int beamID = gid.x;
                int z = gid.y;

                if (beamID >= ysize || z >= zsize) return;

                // % Amplitude
                int idx = beamID * zsize + z;
                float percentAmplitude = abs(ascanData[idx] - u_amplitudeParams.x) 
                                       / u_amplitudeParams.y * u_amplitudeParams.z;

                // Color
                vec3 color = colors[int(clamp(percentAmplitude * 255.0, 0.0, 255.0))];

                // Raster pixel
                ivec2 pixel = sectorialPoints[idx];
                if (pixel.x >= 0 && pixel.x < imageSize(img_output).x &&
                    pixel.y >= 0 && pixel.y < imageSize(img_output).y) {
                    imageStore(img_output, pixel, vec4(color, 1.0));
                }
            }

        )");
        computeShader->link();
    }
    computeShader->bind();

    // ****************************************************************************************
    // Create and upload texture class nObserver: public QOpenGLFunctions_4_3_Core {

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1270, 480);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    // Prepare and upload ascanData buffer
    std::vector<int> flattenedAscanData;
    for (int beamID = 0; beamID < ysize; ++beamID) {
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        flattenedAscanData.insert(flattenedAscanData.end(), ascanData, ascanData + zsize);
    }

    GLuint ascanBuffer;
    glGenBuffers(1, &ascanBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ascanBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, flattenedAscanData.size() * sizeof(int), flattenedAscanData.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ascanBuffer);

    // Prepare and upload sectorialPoints buffer
    std::vector<cv::Point> flattenedSectorialPoints;
    for (int beamID = 0; beamID < ysize; ++beamID) {
        flattenedSectorialPoints.insert(flattenedSectorialPoints.end(), xySectorial[beamID].begin(), xySectorial[beamID].end());
    }

    
    glGenBuffers(1, &sectorialBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sectorialBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, flattenedSectorialPoints.size() * sizeof(cv::Point), flattenedSectorialPoints.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sectorialBuffer);

    // Prepare and upload color palette buffer
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, colorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, everyColors.size() * sizeof(Color), everyColors.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, colorBuffer);

    // Set uniforms
    glUniform3f(glGetUniformLocation(computeShader->programId(), "u_amplitudeParams"), minAmplitude, maxAmplitudeSampling, maxAmplitudeUsable);
    glUniform1i(glGetUniformLocation(computeShader->programId(), "zsize"), zsize);
    glUniform1i(glGetUniformLocation(computeShader->programId(), "ysize"), ysize);

    // Dispatch compute shader
    const int workGroupSizeX = 16;
    const int workGroupSizeY = 16;
    glDispatchCompute((ysize + workGroupSizeX - 1) / workGroupSizeX,
        (zsize + workGroupSizeY - 1) / workGroupSizeY, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    vertice_sview.clear();
    for (int beamID = 0; beamID < ysize; ++beamID) {
        for (int z = 0; z < zsize; ++z) {
            VertexData vertex;
            const cv::Point& point = xySectorial[beamID][z]; 
            vertex.position = QVector2D(point.x, point.y); 
            vertex.color = QVector3D(1.0f, 0.0f, 0.0f);
            nObserver::vertice_sview.append(vertex);
        }
    }
    std::cout << "vertice_sview size: " << nObserver::vertice_sview.size() << std::endl;
    // Clean up resources
    computeShader->release();
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &ascanBuffer);
    glDeleteBuffers(1, &sectorialBuffer);
    glDeleteBuffers(1, &colorBuffer);

}


void nObserver::upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection)
{
    if (!_nAscanCollection) return;
    if (nAscanCollection.size() > 200) {
        nAscanCollection.clear();
        throw std::runtime_error("buffer overflow");
        return;
    }
    nAscanCollection.push_back(_nAscanCollection);
}


