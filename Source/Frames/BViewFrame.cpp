#include "BviewFrame.h"

BviewFrame::BviewFrame(QWidget* parent): QOpenGLWidget(parent), surface(new QOffscreenSurface)
{
    QOpenGLWidget::setUpdateBehavior(QOpenGLWidget::PartialUpdate);
}

QWidget* BviewFrame::createFrame() {
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

void BviewFrame::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.3f, 0.2f, 0.4f, 1.0f);
    if (nIsGlTexture)
    {
        if (!shaderProgram) {
            sttlogs->logInfo("Start initialize OpenGL - GLTexture on GPU for BviewFrame");
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
                #version 330 core
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
                #version 330 core
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
void BviewFrame::paintGL() {
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
    if (nIsGlTexture && ArtScan->BViewBuf) // Check if OpenGL texture rendering is enabled and SViewBuf is valid
    {
        {
            std::lock_guard<std::mutex> lock(ArtScanMutex);
            orgimage = std::make_shared<cv::Mat>(ArtScan->BViewBuf->clone());
            //ArtScan->BViewBuf = nullptr;
            if (orgimage->size().width == 0 || orgimage->size().height == 0) return;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, orgimage->cols, orgimage->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, orgimage->data);
        float aspectRatio = 1.0f;
        if (ArtScan->SViewBuf->rows != 0) float aspectRatio = static_cast<float>(ArtScan->SViewBuf->cols) / ArtScan->SViewBuf->rows;
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

void BviewFrame::updateRealTime()
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
void BviewFrame::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "Bview ReSize: " << width << "x" << height << std::endl;
}


void BviewFrame::updateOffLine() {
    if (isRealTime) isRealTime = false;
    if (scandat.Amplitudes.empty()) return;

    CreateArtFrame();    
    addPoints(true, -1, -1);
}
void BviewFrame::CreateArtFrame()
{
    if (scandat.Amplitudes.empty()) return;

    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(zsize, xsize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    auto everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);

    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t x = 0; x < xsize; ++x) {
            uint64_t index = z * (xsize * ysize) + curpt.y * xsize + x;
            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index));
                return;
            }

            int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(z, x) = cv::Vec3b(color.B, color.G, color.R);
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
void BviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    for (auto item : scene->items()) {
        if (item->data(0).toString() == "MarkLine") {
            scene->removeItem(item);
            delete item;
            break;
        }
    }

    auto* MarkLine = new QGraphicsItemGroup();
    MarkLine->setData(0, "MarkLine");
    scene->addItem(MarkLine);

    QPen gpen(Qt::blue);
    gpen.setWidth(3);
    gpen.setCosmetic(true);

    double pixelX = (Cviewlink) ? static_cast<double>(curpt.x) * scaledImage->cols / xsize : static_cast<double>(x);
    double pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);

    auto* verticalLine = new QGraphicsLineItem(pixelX, 0, pixelX, scaledImage->rows);
    verticalLine->setPen(gpen);
    MarkLine->addToGroup(verticalLine);

    if (overlay) overlay->updatePoints(pixelX, pixelZ, Qt::blue, Qt::color0);

    graphicsView->update();
}
std::pair<int, int> BviewFrame::calculateOriginalPos(int scaled_x, int scaled_z) {
    if (orgimage == nullptr || scaledImage == nullptr) throw std::exception();

    int original_x = static_cast<int>(scaled_x * static_cast<double>(orgimage->cols) / scaledImage->cols);
    int original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);

    if (original_x < 0 || original_z < 0 || original_z >= zsize || original_x >= xsize) throw std::exception();
    return { original_x, original_z };
}
void BviewFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_x, int scaled_z) {
        try
        {
            auto [original_x, original_z] = calculateOriginalPos(scaled_x, scaled_z);
            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(original_x).arg(curpt.y).arg(original_z);
            QToolTip::showText(QCursor::pos(), tooltipText);
            overlay->updateOverlay(scaled_x, scaled_z, scaledImage->cols, scaledImage->rows);
            graphicsView->update();

        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_x, int scaled_z) {
        try
        {
            std::tie(curpt.x, curpt.z) = calculateOriginalPos(scaled_x, scaled_z);

            isPanning = false;
            addPoints(false, scaled_x, scaled_z);

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


