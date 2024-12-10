#include "..\pch.h"
#include "ObserverMgr.h"
#include "SystemConfig/ConfigLocator.h"
// Static variables *******************************************************************************
    nmainUI::statuslogs* nObserver::sttlogs = nullptr;
    ProcessingContext nObserver::prosdt = ProcessingContext();

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
    if (!prosdt.ArtScan) prosdt.ArtScan = &UIArtScan::getInstance();
    everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
}

void preXYsectorial(
    int gysize, int gzsize, double gbeamStartAngle, double gangleDefault,
    double gmaxRadius, int gcenterX, int gcenterY,
    std::vector<std::vector<cv::Point>>& gxySectorial)
{
    static int lastYSize = -1;
    static int lastZSize = -1;

    if (gysize != lastYSize || gzsize != lastZSize) {
        gxySectorial.clear();
        gxySectorial.resize(gysize, std::vector<cv::Point>(gzsize));
        std::cout << "precalculate for prosdt.xySectorial with size: " << gysize << "x" << gzsize << std::endl;

        for (int beamID = 0; beamID < gysize; ++beamID) {
            double angle = gbeamStartAngle + gangleDefault + beamID;
            double radian = angle * CV_PI / 180.0;
            double cosValue = cos(radian);
            double sinValue = sin(radian);

            for (int z = 0; z < gzsize; ++z) {
                double radius = gmaxRadius * (static_cast<double>(z) / gzsize);
                int x = gcenterX + static_cast<int>(radius * cosValue);
                int y = gcenterY - static_cast<int>(radius * sinValue);
                gxySectorial[beamID][z] = cv::Point(x, y);
            }
        }

        lastYSize = gysize;
        lastZSize = gzsize;
    }
}

// Process Func *******************************************************************************
void nObserver::updateParameters(std::shared_ptr<IAscanCollection>& RawAsanDat) {


    std::shared_lock<std::shared_mutex> lock(collectionMutex);

    // Clear remaining data if necessary
    if (prosdt.nAscanCollection.size() > 0) {
        while (!prosdt.nAscanCollection.empty()) {
            prosdt.nAscanCollection.pop_front();
        }
        prosdt.zsize = -1;
        return;
    }

    // Update all parameters
    prosdt.zsize = static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity());
    RawAsanDat->GetAscan(0)->GetAmplitudeDataRange()->GetUnit();
    prosdt.ysize = static_cast<int>(RawAsanDat->GetCount());
    prosdt.angStart = oms.OMS->BeamStartAngle;
    prosdt.angEnd = oms.OMS->beamNumber + oms.OMS->BeamStartAngle;
    prosdt.pautmode = ConfigL->visualConfig->setPautMode;

    // Reset and clear all render buffer
    prosdt.ArtScan->resetAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    prosdt.xySectorial.clear();
    prosdt.xySectorial.resize(prosdt.ysize, std::vector<cv::Point>(prosdt.zsize));

    // Precalculate sectorial coordinates
    int centerX = prosdt.angStart < 0 ? (prosdt.zsize * sin(((-prosdt.angStart) * M_PI / 180))) : 0;
    for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
        double angle = prosdt.angStart - 90 + beamID;
        double radian = angle * CV_PI / 180.0;
        double cosValue = cos(radian);
        double sinValue = sin(radian);

        for (int z = 0; z < prosdt.zsize; ++z) {
            double radius = prosdt.zsize * (static_cast<double>(z) / prosdt.zsize);
            int x = centerX + static_cast<int>(radius * cosValue);
            int y = 0 - static_cast<int>(radius * sinValue);
            prosdt.xySectorial[beamID][z] = cv::Point(x, y);
        }
    }

    prosdt.sview_x = (prosdt.pautmode == PautModeOmni::Linear) ?
        prosdt.ysize + std::abs(sin(prosdt.angStart * M_PI / 180) * prosdt.zsize) :
        prosdt.angStart < 0 ?
        (sin(((-prosdt.angStart) * M_PI / 180)) + sin((prosdt.angEnd)*M_PI / 180)) * prosdt.zsize :
        sin((prosdt.angEnd)*M_PI / 180) * prosdt.zsize;

    prosdt.minAmplitude = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
    prosdt.maxAmplitudeSampling = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
    prosdt.maxAmplitudeUsable = RawAsanDat->GetAscan(0)->GetAmplitudeDataRange()->GetFloatingMax();

    sttlogs->logInfo("Update Amplitude Range: \nminAmplitude: " + std::to_string(prosdt.minAmplitude) +
        " prosdt.maxAmplitudeSampling: " + std::to_string(prosdt.maxAmplitudeSampling) +
        " prosdt.maxAmplitudeUsable: " + std::to_string(prosdt.maxAmplitudeUsable));

    IOmConnect::isUpdate = false;
}
void nObserver::RealDatProcess() {
    shared_ptr<IAscanCollection> RawAsanDat;  

    static const uint16_t BCsize = 1000;

    cv::Mat tempSViewBuf, tempCViewBuf, tempBViewBuf;
    {
        std::shared_lock<std::shared_mutex> lock(collectionMutex);
        if (prosdt.nAscanCollection.empty() || !prosdt.nAscanCollection.front()) {
            if (!prosdt.nAscanCollection.empty()) {
                prosdt.nAscanCollection.pop_front();
            }
            return;
        }
        RawAsanDat = std::move(prosdt.nAscanCollection.front());
        prosdt.nAscanCollection.pop_front();
    }

    // Update
    if (IOmConnect::isUpdate || prosdt.zsize == -1) {
        updateParameters(RawAsanDat);
        return;
    }

    // Process
    tempSViewBuf = cv::Mat::zeros(prosdt.zsize, prosdt.sview_x, CV_8UC3);
    tempSViewBuf.setTo(cv::Scalar(0, 0, 0, 0));


    if (!prosdt.ArtScan->CViewBuf || prosdt.ArtScan->CViewBuf->empty() || !prosdt.ArtScan->BViewBuf || prosdt.ArtScan->BViewBuf->empty()) {
        tempCViewBuf = cv::Mat::zeros(prosdt.ysize, BCsize, CV_8UC3);
        tempBViewBuf = cv::Mat::zeros(prosdt.zsize, BCsize, CV_8UC3);
    }
    else {
        if (prosdt.ArtScan->BViewBuf->rows < prosdt.zsize || prosdt.ArtScan->BViewBuf->cols < prosdt.ysize) {
            tempCViewBuf = cv::Mat::zeros(prosdt.ysize, BCsize, CV_8UC3);
            tempBViewBuf = cv::Mat::zeros(prosdt.zsize, BCsize, CV_8UC3);
        }
        else
        {
            prosdt.ArtScan->CViewBuf->copyTo(tempCViewBuf);
            prosdt.ArtScan->BViewBuf->copyTo(tempBViewBuf);
        }
    }
    std::vector<glm::vec2> points(prosdt.zsize);
    //check if prosdt.zsize > SampleQuantity: return for update
    if (std::abs(prosdt.zsize - static_cast<int>(RawAsanDat->GetAscan(0)->GetSampleQuantity())) > 2) {
        CoutLogger()->critical("Process: prosdt.zsize > Sample Quantity. Need to Update");
        prosdt.zsize = -1; return;
    }

#pragma omp parallel for
    for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
        double maxAmplitudeCscan = 0;
        Color maxColor{};
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        for (int z = 0; z < prosdt.zsize; ++z) {
            double percentAmplitude = std::abs(ascanData[z] - prosdt.minAmplitude) / prosdt.maxAmplitudeSampling * prosdt.maxAmplitudeUsable;
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            int adjustedX{ -1 }; 
            if (prosdt.pautmode == PautModeOmni::Linear) {
                int offsetX = std::abs(static_cast<int>(z * sin(prosdt.angStart * M_PI / 180)));
                adjustedX = beamID + offsetX;
                if (adjustedX >= 0 && adjustedX < tempSViewBuf.cols && z >= 0 && z < tempSViewBuf.rows) {
                    auto& pixel = tempSViewBuf.at<cv::Vec3b>(z, adjustedX);
                    pixel = cv::Vec3b(color.B, color.G, color.R);
                }                
            }
            
            else if (prosdt.pautmode == PautModeOmni::Sectorial || prosdt.pautmode == PautModeOmni::Compound) {
                const cv::Point& point = prosdt.xySectorial[beamID][z];
                adjustedX = point.x;
                int y = point.y;

                if (adjustedX >= 0 && adjustedX < tempSViewBuf.cols && y >= 0 && y < tempSViewBuf.rows) {
                    uchar* row_ptr = tempSViewBuf.ptr<uchar>(y);
                    row_ptr[adjustedX * 3 + 0] = color.B;
                    row_ptr[adjustedX * 3 + 1] = color.G;
                    row_ptr[adjustedX * 3 + 2] = color.R;
                }
                if (beamID + 1 < prosdt.ysize)
                {
                    const cv::Point& nextPoint = prosdt.xySectorial[beamID + 1][z];
                    // currently use too much time to process, low performance TODO optimize for highest resolution
                    cv::line(tempSViewBuf, point, nextPoint, cv::Scalar(color.B, color.G, color.R), 1, cv::LINE_4);
                }
            }
            
            if (percentAmplitude > maxAmplitudeCscan) {
                maxAmplitudeCscan = percentAmplitude;
                maxColor = color;
            }

            if (oms.OMS->beamCurrentID == beamID) {
                points[z] = glm::vec2((percentAmplitude / 50.0) - 1.0, -1.0 + 2.0 * static_cast<double>(z) / static_cast<double>(prosdt.zsize - 1));
                tempBViewBuf.at<cv::Vec3b>(z, 0) = cv::Vec3b(color.B, color.G, color.R);
                if ( z == prosdt.zsize - 1) //TODO: bring to GPU; need to convert cv pos into gltexture2D quad
                {
                    if (prosdt.pautmode == PautModeOmni::Linear)
                        cv::line(tempSViewBuf, cv::Point(beamID, 0), cv::Point(adjustedX, tempSViewBuf.rows), cv::Scalar(10, 10, 160, 256), 1, cv::LINE_4);
                    else if (prosdt.pautmode == PautModeOmni::Sectorial || prosdt.pautmode == PautModeOmni::Compound)
                        cv::line(tempSViewBuf, prosdt.xySectorial[0][0], prosdt.xySectorial[beamID][prosdt.zsize - 1], cv::Scalar(10, 10, 160, 256), 1, cv::LINE_4);
                }
            }
        }
        if (beamID >= tempCViewBuf.rows) { prosdt.zsize = -1; return; }
        tempCViewBuf.at<cv::Vec3b>(beamID, 0) = cv::Vec3b(maxColor.B, maxColor.G, maxColor.R);
    }
    
    cv::GaussianBlur(tempSViewBuf, tempSViewBuf, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    if (prosdt.angStart < 0 && prosdt.pautmode == PautModeOmni::Linear) cv::flip(tempSViewBuf, tempSViewBuf, 1);
    {
        std::lock_guard<std::mutex> lock(ArtScanMutex);
        if (prosdt.ArtScan->SViewBuf) { prosdt.ArtScan->SViewBuf = nullptr; }
        prosdt.ArtScan->SViewBuf = std::make_shared<cv::Mat>(tempSViewBuf);
        prosdt.ArtScan->CViewBuf = std::make_shared<cv::Mat>(tempCViewBuf);
        prosdt.ArtScan->CViewBuf->colRange(0, prosdt.ArtScan->CViewBuf->cols - 1).copyTo(tempCViewBuf.colRange(1, tempCViewBuf.cols));

        prosdt.ArtScan->BViewBuf = std::make_shared<cv::Mat>(tempBViewBuf);
        prosdt.ArtScan->BViewBuf->colRange(0, prosdt.ArtScan->BViewBuf->cols - 1).copyTo(tempBViewBuf.colRange(1, tempBViewBuf.cols));
        *prosdt.ArtScan->AViewBuf = points;
    }
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

    cv::Mat tempSViewBuf, tempCViewBuf, tempBViewBuf;
    {
        std::shared_lock<std::shared_mutex> lock(collectionMutex);
        if (prosdt.nAscanCollection.empty() || !prosdt.nAscanCollection.front()) {
            if (!prosdt.nAscanCollection.empty()) {
                prosdt.nAscanCollection.pop_front();
            }
            return;
        }
        RawAsanDat = std::move(prosdt.nAscanCollection.front());
        prosdt.nAscanCollection.pop_front();
    }

    // ****************************************************************************************
    // Update
    if (IOmConnect::isUpdate || prosdt.zsize == -1) {
        updateParameters(RawAsanDat);
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
            #version 430
            layout(local_size_x = 16, local_size_y = 16) in;
            layout(std430, binding = 0) buffer InputData {
                vec2 prosdt.xySectorial[];
            };

            layout(std430, binding = 1) buffer OutputData {
                vec4 outputColor[];
            };

            uniform int prosdt.ysize;
            uniform int prosdt.zsize;

            void main() {
                uint beamID = gl_GlobalInvocationID.x;
                uint z = gl_GlobalInvocationID.y;

                if (beamID < prosdt.ysize && z < prosdt.zsize) {
                    // Perform calculations for prosdt.xySectorial and outputColor
                    vec2 point = prosdt.xySectorial[beamID * prosdt.zsize + z];
                    outputColor[beamID * prosdt.zsize + z] = vec4(point.x, point.y, 0.0, 1.0);
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
    for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
        const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
        flattenedAscanData.insert(flattenedAscanData.end(), ascanData, ascanData + prosdt.zsize);
    }

    GLuint ascanBuffer;
    glGenBuffers(1, &ascanBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ascanBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, flattenedAscanData.size() * sizeof(int), flattenedAscanData.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ascanBuffer);

    // Prepare and upload sectorialPoints buffer
    std::vector<cv::Point> flattenedSectorialPoints;
    for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
        flattenedSectorialPoints.insert(flattenedSectorialPoints.end(), prosdt.xySectorial[beamID].begin(), prosdt.xySectorial[beamID].end());
    }

    
    glGenBuffers(1, &prosdt.sectorialBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, prosdt.sectorialBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, flattenedSectorialPoints.size() * sizeof(cv::Point), flattenedSectorialPoints.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, prosdt.sectorialBuffer);

    // Prepare and upload color palette buffer
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, colorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, everyColors.size() * sizeof(Color), everyColors.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, colorBuffer);

    // Set uniforms
    glUniform3f(glGetUniformLocation(computeShader->programId(), "u_amplitudeParams"), prosdt.minAmplitude, prosdt.maxAmplitudeSampling, prosdt.maxAmplitudeUsable);
    glUniform1i(glGetUniformLocation(computeShader->programId(), "prosdt.zsize"), prosdt.zsize);
    glUniform1i(glGetUniformLocation(computeShader->programId(), "prosdt.ysize"), prosdt.ysize);

    // Dispatch compute shader
    const int workGroupSizeX = 16;
    const int workGroupSizeY = 16;
    glDispatchCompute((prosdt.ysize + workGroupSizeX - 1) / workGroupSizeX,
        (prosdt.zsize + workGroupSizeY - 1) / workGroupSizeY, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    prosdt.vertice_sview.clear();
    for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
        for (int z = 0; z < prosdt.zsize; ++z) {
            VertexData vertex;
            const cv::Point& point = prosdt.xySectorial[beamID][z]; 
            vertex.position = QVector2D(point.x, point.y); 
            vertex.color = QVector3D(1.0f, 0.0f, 0.0f);
            prosdt.vertice_sview.append(vertex);
        }
    }
    std::cout << "vertice_sview size: " << prosdt.vertice_sview.size() << std::endl;
    // Clean up resources
    computeShader->release();
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &ascanBuffer);
    glDeleteBuffers(1, &prosdt.sectorialBuffer);
    glDeleteBuffers(1, &colorBuffer);

}

void nObserver::upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection)
{
    if (!_nAscanCollection) return;
    if (prosdt.nAscanCollection.size() > 200) {
        prosdt.nAscanCollection.clear();
        throw std::runtime_error("buffer overflow");
        return;
    }
    prosdt.nAscanCollection.push_back(_nAscanCollection);
}


