#include "../pch.h"
#include "CviewFrame.h"
//#include <opencv2/cudawarping.hpp> 

CviewFrame::CviewFrame(QWidget* parent): QOpenGLWidget(parent), surface(new QOffscreenSurface)
{
    QOpenGLWidget::setUpdateBehavior(QOpenGLWidget::PartialUpdate);
}
QWidget* CviewFrame::createFrame(){
    if (!graphicsView) {
        graphicsView = std::make_shared<ZoomableGraphicsView>();
    }
    if (!scene) {
        scene = std::make_shared<QGraphicsScene>();
    }
    graphicsView->setScene(scene.get());
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    MouseGetPosXY();

    layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(this);
    frame->setLayout(layout);
    return frame;

}

void CviewFrame::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.3f, 0.4f, 0.0f, 1.0f);
    if (nIsGlTexture)
    {
        if (!shaderProgram) {
            sttlogs->logInfo("Start initialize OpenGL - GLTexture on GPU for CviewFrame");
            // Create texture
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);


            // Load and compile shaders
            shaderProgram = std::make_unique<QOpenGLShaderProgram>();
            shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                R"(
                #version 430 core
                layout (location = 0) in vec2 position;
                layout (location = 1) in vec2 texCoord;

                out vec2 fragTexCoord;

                void main() {
                    gl_Position = vec4(position, 0.0, 1.0);
                    fragTexCoord = vec2( texCoord.x, 1.0 - texCoord.y);

                }
                )");
            shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                R"(
                #version 430 core
                in vec2 fragTexCoord;
                out vec4 FragColor;

                uniform sampler2D u_Texture;

                void main() {
                    FragColor = texture(u_Texture, fragTexCoord);
                }
                )");
            shaderProgram->link();

        }
    }
    connect(this, &QOpenGLWidget::frameSwapped, this, [&]() {
        QOpenGLWidget::update();
        });
    QOpenGLWidget::update();
}
void CviewFrame::paintGL() {
    static QOpenGLContext* context = QOpenGLContext::currentContext();
    if (!context) {
        sttlogs->logCritical("OpenGL context is not valid");
        return;
    }
    static QElapsedTimer fpsTimer; static int frameCount = 0;
    auto ftime = FPS_Calc(fpsTimer, frameCount);
    if (ftime > 0)
    {
        ReadStatus::getinstance().set_sviewfps(ftime);
    }
    if (nIsGlTexture && ArtScan->CViewBuf) // Check if OpenGL texture rendering is enabled and SViewBuf is valid
    {
        {
            std::lock_guard<std::mutex> lock(ArtScanMutex); 
            orgimage = std::make_shared<cv::Mat>(ArtScan->CViewBuf->clone()); 
            //ArtScan->CViewBuf = nullptr;
            if (orgimage->size().width == 0 || orgimage->size().height == 0) return;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, orgimage->cols, orgimage->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, orgimage->data); 
        shaderProgram->bind(); //

        static const GLfloat vertices[] = {
            -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left corner
             1.0f, -1.0f ,  1.0f, 0.0f, // Bottom-right corner
             1.0f,  1.0f ,  1.0f, 1.0f, // Top-right corner
            -1.0f,  1.0f ,  0.0f, 1.0f  // Top-left corner

        };
        static const GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);


        shaderProgram->release(); // Release the shader program
    }
    else if (!nIsGlTexture)
    {
        return;
        glPointSize(5.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shaderProgram->bind();

        shaderProgram->setUniformValue("u_Scale", QVector2D(1, 1));

        if (!vertice_cview.isEmpty()) {
            vao.bind();
            vbo.bind();

            // Update vertex buffer data
            size_t dataSize = vertice_cview.size() * sizeof(VertexData);
            glBufferData(GL_ARRAY_BUFFER, dataSize, vertice_cview.constData(), GL_DYNAMIC_DRAW);

            // Draw points
            glDrawArrays(GL_POINTS, 0, vertice_cview.size());

            vao.release();
            vbo.release();
        }

        shaderProgram->release();
        glFlush();
        glFinish(); 
    }
}

void CviewFrame::updateRealTime()
{
    if (!isRealTime)
    {
        if (graphicsView) {
            layout->removeWidget(graphicsView.get());
            graphicsView->hide();
        }
        layout->addWidget(this);
        this->show();
        isRealTime = true;
    }
}
void CviewFrame::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
#ifndef _DEBUG
    std::cout << "Cview ReSize: " << width << "x" << height << std::endl;
#endif
}

// ********************************************
// Offline Reading
// TODO: Optimize into GPU Rendering
void CviewFrame::updateOffLine() {
    if (isRealTime)
    {
        layout->removeWidget(this);
        this->hide();
        layout->addWidget(graphicsView.get());
        graphicsView->show();
        isRealTime = false;
    }

    static int lastResolution = -1;
    if (scandat.Amplitudes.empty()) { return; }
    if (lastResolution != ConfigL->sysParams->resolution) {
        lastResolution = ConfigL->sysParams->resolution;
        CreateXYview();
    }
    // if Bscan and Cscan layer
    if (ConfigL->sysParams->resolution) {
        CreateXYview();
    }

    static bool lastIsCscanLayer = false;
    if (lastIsCscanLayer != ConfigL->sysParams->isCscanLayer) {
        lastIsCscanLayer = ConfigL->sysParams->isCscanLayer;
        CreateXYview();
    }
    addPoints(true,-1,-1);
}
void CviewFrame::CreateXYview() {
    // Declaration and initialization
    if (scandat.Amplitudes.empty()) {
        return;
    }

    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(ysize, xsize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    uint64_t z_offset = curpt[2] * (xsize * ysize);
    double percentAmplitude;

    // Processing amplitude data to assign colors
    for (uint64_t y = 0; y < ysize; ++y) {
        for (uint64_t x = 0; x < xsize; ++x) {
            if (!ConfigL->sysParams->isCscanLayer) {
                int16_t maxAmplitude = 0;
                for (uint64_t z = 0; z < zsize; ++z) {
                    uint64_t index = z * (xsize * ysize) + y * xsize + x;
                    if (index >= scandat.Amplitudes.size()) {
                        sttlogs->logWarning("Out of range data: " + std::to_string(index) + " / " + std::to_string(scandat.Amplitudes.size()));
                        return;
                    }
                    int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
                    if (samplingAmplitude > maxAmplitude) {
                        maxAmplitude = samplingAmplitude;
                    }
                }
                percentAmplitude = maxAmplitude / (32768.0 / 100.0);
            }
            else {
                uint64_t index = z_offset + y * xsize + x;
                if (index >= scandat.Amplitudes.size()) {
                    sttlogs->logWarning("Out of range data: " + std::to_string(index) + " / " + std::to_string(scandat.Amplitudes.size()));
                    return;
                }
                int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
                percentAmplitude = samplingAmplitude / (32768.0 / 100.0);
            }

            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(y, x) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(orgimage->cols) / static_cast<double>(orgimage->rows);

    auto newWidth = (frameRatio > imageRatio) ? static_cast<int>(orgimage->rows * frameRatio) : orgimage->cols;
    auto newHeight = (frameRatio > imageRatio) ? orgimage->rows : static_cast<int>(orgimage->cols / frameRatio);

    auto scaleFactor = (!isPanning || ConfigLocator::getInstance().settingconf->bhighResBscan) ? 1 : 1.0;
    cv::resize(*orgimage, *scaledImage, cv::Size(newWidth * scaleFactor, newHeight * scaleFactor), 0, 0, cv::INTER_LINEAR);

    cv::GaussianBlur(*scaledImage, *scaledImage, cv::Size(1, 1), 0);
    auto qImage = std::make_shared<QImage>(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();

    QPixmap pixmap = QPixmap::fromImage(*qImage);
    for (auto item : scene->items()) {
        if (item->data(0).toString() == "artwork") {
            scene->removeItem(item);
            delete item;
            break;
        }
    }
    QGraphicsPixmapItem* artworkItem = scene->addPixmap(pixmap);
    artworkItem->setData(0, "artwork");

    static bool first_flag = true; if (first_flag) graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio); first_flag = false;
    graphicsView->update();
}
void CviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    double pixelX= (Cviewlink) ? static_cast<double>(curpt[0]) * scaledImage->cols / xsize : static_cast<double>(x);
    double pixelY = (Cviewlink) ? static_cast<double>(curpt[1]) * scaledImage->rows / ysize : static_cast<double>(y);

    if (overlay) { overlay->updatePoints(pixelX, pixelY, Qt::blue, Qt::red); }
    graphicsView->update();
}
std::pair<int, int> CviewFrame::calculateOriginalPos(int scaled_x, int scaled_y) {
    if (orgimage == nullptr || scaledImage == nullptr) {
        throw std::exception();
    }
    auto scale_x = static_cast<double>(orgimage->cols) / static_cast<double>(scaledImage->cols);
    auto scale_y = static_cast<double>(orgimage->rows) / static_cast<double>(scaledImage->rows);

    int original_x = static_cast<int>(scaled_x * scale_x);
    int original_y = static_cast<int>(scaled_y * scale_y);

    if (original_x < 0 || original_y < 0 || original_y >= ysize || original_x >= xsize) {
        throw std::exception();
    }
    return { original_x, original_y };
}
void CviewFrame::MouseGetPosXY()
{
    static int temX{ 0 }, temY{ 0 };
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_x, int scaled_y) {
        try
        {
            temX = scaled_x; temY = scaled_y;
            auto [original_x, original_y] = calculateOriginalPos(scaled_x, scaled_y);
            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(original_x).arg(original_y).arg(curpt[2]);
            QToolTip::showText(QCursor::pos(), tooltipText);
            overlay->updateOverlay(scaled_x, scaled_y, scaledImage->cols, scaledImage->rows);
            graphicsView->update();

        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_x, int scaled_y) {
        try
        {
            temX = scaled_x; temY = scaled_y;
            std::tie(curpt[0], curpt[1]) = calculateOriginalPos(scaled_x, scaled_y);
            isPanning = false;

            addPoints(false, scaled_x, scaled_y);
        }
        catch (...) { (void)0; }
        });
    

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseLeftView, [=]() {
        overlay->ClearLineGroup();
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::nKeyPressedEvent, [=]() {
        for (auto item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                graphicsView->fitInView(item->boundingRect(), Qt::KeepAspectRatio);
                break;
            }
        }

        });
}