#include "..\pch.h"
#include "ObserverMgr.h"
#include "lines_util.hpp"
nmainUI::statuslogs* nObserver::sttlogs = nullptr;
ProcessingContext nObserver::prosdt = ProcessingContext();
std::shared_mutex nObserver::collectionMutex = std::shared_mutex();
std::mutex nObserver::ArtScanMutex = std::mutex();
std::unique_ptr<QOpenGLShaderProgram> upFrame::computeShader = nullptr;
std::unique_ptr<QOffscreenSurface> upFrame::surface = nullptr;
std::unique_ptr<QOpenGLContext> upFrame::context = nullptr;

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
static const uint16_t BCsize = 1000;

// Process Func
void nObserver::RealDatProcess() {
    shared_ptr<IAscanCollection> RawAsanDat;  

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

    std::vector<Line> linesToDraw;
    linesToDraw.reserve(prosdt.ysize * prosdt.zsize);
    Color maxColor{}; int getx{ -1 }, gety{ -1 };
    double maxAmplitudeCscan = -1;
#pragma omp parallel
    {
        std::vector<Line> localLines;
#pragma omp for nowait
        for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
            const int* ascanData = RawAsanDat->GetAscan(beamID)->GetData();
            double maxAmplitudeCscan = -1.0;
            Color maxColor;

            for (int z = 0; z < prosdt.zsize; ++z) {
                double percentAmplitude = std::abs(ascanData[z] - prosdt.minAmplitude) / prosdt.maxAmplitudeSampling * prosdt.maxAmplitudeUsable;
                Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
                // Sview
                if (prosdt.pautmode == PautModeOmni::Linear) {
                    int getx = prosdt.xyLinear[beamID][z].x;
                    if (getx >= 0 && getx < tempSViewBuf.cols && z >= 0 && z < tempSViewBuf.rows) {
                        setPixelFast(tempSViewBuf, getx, z, cv::Vec3b(color.B, color.G, color.R));
                    }
                    if (beamID + 1 < prosdt.ysize) {
                        cv::Point point = prosdt.xyLinear[beamID][z];
                        cv::Point nextPoint = prosdt.xyLinear[beamID + 1][z];
                        localLines.push_back(Line{ point, nextPoint, cv::Vec3b(color.B, color.G, color.R) });
                    }
                }
                else if (prosdt.pautmode == PautModeOmni::Sectorial || prosdt.pautmode == PautModeOmni::Compound) {
                    int getx = prosdt.xySectorial[beamID][z].x;
                    int gety = prosdt.xySectorial[beamID][z].y;

                    if (getx >= 0 && getx < tempSViewBuf.cols && gety >= 0 && gety < tempSViewBuf.rows) {
                        setPixelFast(tempSViewBuf, getx, gety, cv::Vec3b(color.B, color.G, color.R));
                    }

                    if (beamID + 1 < prosdt.ysize) {
                        cv::Point point = prosdt.xySectorial[beamID][z];
                        cv::Point nextPoint = prosdt.xySectorial[beamID + 1][z];
                        localLines.push_back(Line{ point, nextPoint, cv::Vec3b(color.B, color.G, color.R) });
                    }
                }
                // Cview calc
                if (percentAmplitude > maxAmplitudeCscan) {
                    maxAmplitudeCscan = percentAmplitude;
                    maxColor = color;
                }
                // Bview && Aview
                if (oms.OMS->beamCurrentID == beamID) {
                    points[z] = glm::vec2((percentAmplitude / 50.0) - 1.0, -1.0 + 2.0 * static_cast<double>(z) / static_cast<double>(prosdt.zsize - 1));
                    setPixelFast(tempBViewBuf, 0, z, cv::Vec3b(color.B, color.G, color.R));

                    
                }
            }
            // Cview
            if (beamID >= tempCViewBuf.rows) {
                prosdt.zsize = -1;
                return;
            }
            setPixelFast(tempCViewBuf, 0, beamID, cv::Vec3b(maxColor.B, maxColor.G, maxColor.R));
        }
#pragma omp critical
        { linesToDraw.insert(linesToDraw.end(), localLines.begin(), localLines.end()); }
    }

#pragma omp parallel for
    for (size_t i = 0; i < linesToDraw.size(); ++i) {
        drawLineOptimized(tempSViewBuf, linesToDraw[i].start.x, linesToDraw[i].start.y,
            linesToDraw[i].end.x, linesToDraw[i].end.y, linesToDraw[i].color);
    }

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(tempSViewBuf, tempSViewBuf, cv::MORPH_CLOSE, kernel);
    cv::GaussianBlur(tempSViewBuf, tempSViewBuf, cv::Size(5, 5), 0);

    auto beamID = oms.OMS->beamCurrentID;
    if (prosdt.pautmode == PautModeOmni::Linear && beamID < prosdt.ysize) {
        cv::Point startPt(prosdt.xyLinear[beamID][0].x, prosdt.xyLinear[beamID][0].y);
        cv::Point endPt(prosdt.xyLinear[beamID][prosdt.zsize - 1].x, prosdt.xyLinear[beamID][prosdt.zsize - 1].y);
        drawLineOptimized(tempSViewBuf, startPt.x, startPt.y, endPt.x, endPt.y, cv::Vec3b(160, 10, 10));
    }
    else if ((prosdt.pautmode == PautModeOmni::Sectorial || prosdt.pautmode == PautModeOmni::Compound) && beamID < prosdt.ysize){
        cv::Point startPt(prosdt.xySectorial[0][0].x, prosdt.xySectorial[0][0].y);
        cv::Point endPt(prosdt.xySectorial[beamID][prosdt.zsize - 1].x, prosdt.xySectorial[beamID][prosdt.zsize - 1].y);
        drawLineOptimized(tempSViewBuf, startPt.x, startPt.y, endPt.x, endPt.y, cv::Vec3b(160, 10, 10));

    }
    

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


// Init and update parameters
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
    prosdt.xyLinear.clear();
    prosdt.xySectorial.resize(prosdt.ysize, std::vector<cv::Point>(prosdt.zsize));
    prosdt.xyLinear.resize(prosdt.ysize, std::vector<cv::Point>(prosdt.zsize));

    // Precalculate sectorial and linear coordinates
    int centerX = prosdt.angStart < 0 ? (prosdt.zsize * sin(((-prosdt.angStart) * M_PI / 180))) : 0;

    for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
        double angle = prosdt.angStart - 90 + beamID;
        double radian = angle * CV_PI / 180.0;
        double cosValue = cos(radian);
        double sinValue = sin(radian);

        for (int z = 0; z < prosdt.zsize; ++z) {
            double radius = prosdt.zsize * (static_cast<double>(z) / prosdt.zsize);

            // Calculate Sectorial coordinates
            int xSectorial = centerX + static_cast<int>(radius * cosValue);
            int ySectorial = 0 - static_cast<int>(radius * sinValue);
            prosdt.xySectorial[beamID][z] = cv::Point(xSectorial, ySectorial);

            // Calculate Linear coordinates
            int offsetX = std::abs(static_cast<int>(z * sin(prosdt.angStart * M_PI / 180)));
            int xLinear = oms.OMS->SviewScaleX * beamID + offsetX ;
            prosdt.xyLinear[beamID][z] = cv::Point(xLinear , z);

        }
    }
    std::cout << "Size: Beam*Ascan [ " << prosdt.ysize << " x " << prosdt.zsize << " ]" << std::endl;

    prosdt.sview_x = (prosdt.pautmode == PautModeOmni::Linear) ?
        oms.OMS->SviewScaleX * prosdt.ysize + std::abs(sin(prosdt.angStart * M_PI / 180) * prosdt.zsize) : /*Linear*/
            prosdt.angStart < 0 ? /*Not Linear*/
        (sin(((-prosdt.angStart) * M_PI / 180)) + sin((prosdt.angEnd) * M_PI / 180)) * prosdt.zsize :
        sin((prosdt.angEnd) * M_PI / 180) * prosdt.zsize;

    prosdt.minAmplitude = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMin();
    prosdt.maxAmplitudeSampling = RawAsanDat->GetAscan(0)->GetAmplitudeSamplingDataRange()->GetFloatingMax();
    prosdt.maxAmplitudeUsable = RawAsanDat->GetAscan(0)->GetAmplitudeDataRange()->GetFloatingMax();

    sttlogs->logInfo("Update Amplitude Range: \nminAmplitude: " + std::to_string(prosdt.minAmplitude) +
        " prosdt.maxAmplitudeSampling: " + std::to_string(prosdt.maxAmplitudeSampling) +
        " prosdt.maxAmplitudeUsable: " + std::to_string(prosdt.maxAmplitudeUsable));

    IOmConnect::isUpdate = false;
}
void nObserver::processOnGPU()
{
    static std::shared_ptr<upFrame> upframe ;
    if (!upframe) {
        upframe = std::make_shared<upFrame>();
        upframe->createFrame();
    }
    upframe->processOnGPU();
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



void upFrame::processOnGPU()
{
    static bool pointsUploaded = false;
    static bool colorsUploaded = false;

    shared_ptr<IAscanCollection> RawAsanDat;
    cv::Mat tempSViewBuf, tempCViewBuf, tempBViewBuf;

    // Step 1: Fetch data safely
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

    // Step 2: Update parameters if needed
    if (IOmConnect::isUpdate || prosdt.zsize == -1) {
        updateParameters(RawAsanDat);
        pointsUploaded = false; // Mark points for re-upload if parameters changed
        return;
    }

    computeShader->bind();

    // Step 3: Prepare OpenGL resources
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, prosdt.ysize, prosdt.zsize);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

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

    // Step 4: Upload sectorial points only if not already uploaded
    if (!pointsUploaded) {
        std::vector<glm::vec2> flattenedSectorialPoints;
        for (int beamID = 0; beamID < prosdt.ysize; ++beamID) {
            for (int z = 0; z < prosdt.zsize; ++z) {
                flattenedSectorialPoints.emplace_back(
                    (beamID / float(prosdt.ysize)) * 2.0f - 1.0f, // Normalize x
                    (z / float(prosdt.zsize)) * 2.0f - 1.0f        // Normalize y
                );
            }
        }

        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, flattenedSectorialPoints.size() * sizeof(glm::vec2), flattenedSectorialPoints.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        pointsUploaded = true;
    }

    // Step 5: Upload color palette buffer only if not already uploaded
    if (!colorsUploaded) {
        GLuint colorBuffer;
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, colorBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, everyColors.size() * sizeof(Color), everyColors.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, colorBuffer);

        colorsUploaded = true;
    }

    // Step 6: Set uniforms
    glUniform1i(glGetUniformLocation(computeShader->programId(), "prosdt.zsize"), prosdt.zsize);
    glUniform1i(glGetUniformLocation(computeShader->programId(), "prosdt.ysize"), prosdt.ysize);

    // Step 7: Dispatch compute shader
    const int workGroupSizeX = 16;
    const int workGroupSizeY = 16;
    glDispatchCompute((prosdt.ysize + workGroupSizeX - 1) / workGroupSizeX,
        (prosdt.zsize + workGroupSizeY - 1) / workGroupSizeY, 1);

    // Ensure memory barriers for texture updates
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    GLuint err = glGetError();
    if (err != GL_NO_ERROR) {
        qDebug() << "Failed to ensure memory barriers for texture updates: " << err;
    }
    // Bind the result to prosdt.sviewID
    if (glIsTexture(prosdt.sviewID)) {
        glDeleteTextures(1, &prosdt.sviewID); // Delete previous texture if exists
    }
    glGenTextures(1, &prosdt.sviewID);
    glBindTexture(GL_TEXTURE_2D, prosdt.sviewID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, prosdt.ysize, prosdt.zsize);
    glBindImageTexture(0, prosdt.sviewID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Optional: Validate binding
    if (!glIsTexture(prosdt.sviewID)) {
        qDebug() << "Failed to bind compute shader output to prosdt.sviewID.";
    }
    std::vector<float> textureData(prosdt.ysize * prosdt.zsize * 4); // RGBA32F
    glBindTexture(GL_TEXTURE_2D, prosdt.sviewID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, textureData.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    int loggedPixels = 0;
    for (int i = 0; i < int(textureData.size() / 4) && loggedPixels < 10; ++i) {
        float r = textureData[i * 4];
        float g = textureData[i * 4 + 1];
        float b = textureData[i * 4 + 2];
        float a = textureData[i * 4 + 3];

        if (std::abs(r) > 0.1f || std::abs(g) > 0.1f || std::abs(b) > 0.1f || std::abs(a) > 0.1f) {
            qDebug() << "Pixel" << i << ": R=" << r
                << " G=" << g
                << " B=" << b
                << " A=" << a;
            ++loggedPixels;
        }
    }
    if (loggedPixels == 0) {
        qDebug() << "No non-zero pixels found in texture.";
    }


    // Step 8: Clean up resources
    computeShader->release();

    // Don't delete prosdt.sviewID, as it's needed for rendering
    if (texture != prosdt.sviewID) {
        glDeleteTextures(1, &texture);
    }

    glDeleteBuffers(1, &ascanBuffer);

}
QWidget* upFrame::createFrame()
{
    if (!surface) {
        surface = std::make_unique<QOffscreenSurface>();
        surface->setFormat(QSurfaceFormat::defaultFormat());
        surface->create();
        if (!surface->isValid()) {
            throw std::runtime_error("Failed to create QOffscreenSurface");
        }
    }

    if (!context) {
        context = std::make_unique<QOpenGLContext>();
        context->setFormat(surface->format());
        if (!context->create()) {
            throw std::runtime_error("Failed to create QOpenGLContext");
        }
    }

    // Step 2: Make context current and initialize OpenGL functions
    context->makeCurrent(surface.get());

    // Step 3: Create shader if not already created
    if (!computeShader) {
        initializeOpenGLFunctions();
        qDebug() << "OpenGL Version:" << (const char*)glGetString(GL_VERSION);
        qDebug() << "GLSL Version:" << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

        computeShader = std::make_unique<QOpenGLShaderProgram>(QOpenGLContext::currentContext());
        if (!computeShader->addShaderFromSourceCode(QOpenGLShader::Compute,
            R"(
            #version 430
            layout(local_size_x = 16, local_size_y = 16) in;

            layout(binding = 0, rgba32f) uniform image2D outputTexture;

            uniform int ysize;
            uniform int zsize;

            void main() {
                uint beamID = gl_GlobalInvocationID.x;
                uint z = gl_GlobalInvocationID.y;

                if (beamID < ysize && z < zsize) {
                    vec4 color = vec4(1.0, 0.0, 0.0, 1.0); // Entire texture filled with red
                    imageStore(outputTexture, ivec2(beamID, z), color);
                }
            }
            )")) {
            throw std::runtime_error("Failed to add shader source.");
        }

        if (!computeShader->link()) {
            throw std::runtime_error("Failed to link compute shader.");
        }
    }

    return nullptr;
}

