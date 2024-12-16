#include "SviewFrame.h"
#include <omp.h>
#include "SystemConfig/ConfigLocator.h"

SviewFrame::SviewFrame(QWidget* parent)
    : QOpenGLWidget(parent), surface(new QOffscreenSurface)
{
    QOpenGLWidget::setUpdateBehavior(QOpenGLWidget::PartialUpdate); 
}
QWidget* SviewFrame::createFrame() {
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

    layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(this);
    frame->setLayout(layout);
    return frame;
}


void SviewFrame::Wait3DScreen() {
    // start

};
void SviewFrame::initializeGL() {     
    initializeOpenGLFunctions();
    glClearColor(0.09f, 0.09f, 0.09f, 1.0f);
    if (pautmgr->isGLTexture())
    {
        if (!shaderProgram) {
            sttlogs->logInfo("Start initialize OpenGL - GLTexture on GPU for SviewFrame");
            // Create texture
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Load and compile shaders
            shaderProgram = std::make_unique<QOpenGLShaderProgram>();
            shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                R"(
                #version 430 core
                layout (location = 0) in vec3 position;
                layout (location = 1) in vec2 texCoord;

                uniform mat4 mvp;            // Model-View-Projection matrix
                uniform bool is3D;           // Flag: is 3D or 2D

                out vec2 fragTexCoord;                
                void main() {
                    if (is3D) {
                        gl_Position = mvp * vec4(position, 1.0); 
                    } else {
                        gl_Position = vec4(position.xy, 0.0, 1.0); 
                    }
                    fragTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
                }
                )");
            shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                R"(
                #version 430 core
                in vec2 fragTexCoord;
                out vec4 FragColor;

                uniform sampler2D u_Texture;
                uniform vec2 zoomCenter; 
                uniform float zoomRatio;
                
                void main() {
                    vec2 zoomedTexCoord = (fragTexCoord - zoomCenter) * zoomRatio + zoomCenter;
                    FragColor = texture(u_Texture, zoomedTexCoord);
                }
                )");
            shaderProgram->link();
        }
    }
    else
    {
        if (!shaderProgram) {
            sttlogs->logNotify("Start initialize OpenGL - glBufferData on GPU for SviewFrame");
            // Create shader program
            shaderProgram = std::make_unique<QOpenGLShaderProgram>();
            shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                R"(
            #version 430 core
            layout (location = 0) in vec2 position;
            layout (location = 1) in vec2 texCoord;

            out vec2 TexCoord;

            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                TexCoord = texCoord;
            }
            )");
            shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                R"(
            #version 430 core
            in vec2 TexCoord;
            out vec4 FragColor;

            uniform sampler2D u_Texture;

            void main() {
                FragColor = texture(u_Texture, TexCoord);
            }
            )");
            shaderProgram->link();

            // Set up VAO and VBO
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            // Define quad vertices (normalized device coordinates)
            float vertices[] = {
                // Positions    // TexCoords
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f,  0.0f, 1.0f,

                 1.0f, -1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 1.0f,
                -1.0f,  1.0f,  0.0f, 1.0f
            };

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }
    connect(this, &QOpenGLWidget::frameSwapped, this, [&]() {
        QOpenGLWidget::update();
        });
    QOpenGLWidget::update();
}
void SviewFrame::paintGL() {     
    static FrameTimeTracker frameTracker;
    StartFrameTimer(frameTracker); // EndFrameTimer

    // wait screne
    static bool startFlag = false;
    if (!startFlag) {
        Wait3DScreen();
    }
    // Render by GlTexture
    if (pautmgr->isGLTexture() && pautmgr->prosdt.ArtScan->SViewBuf)
    {
        startFlag = true;
        {
            std::lock_guard<std::mutex> lock(pautmgr->ArtScanMutex);
            orgimage = std::make_shared<cv::Mat>(pautmgr->prosdt.ArtScan->SViewBuf->clone());
            pautmgr->prosdt.ArtScan->SViewBuf = nullptr;
            if (orgimage->size().width == 0 || orgimage->size().height == 0) return;
        }
        glBindTexture(GL_TEXTURE_2D, textureID); 
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, orgimage->cols, orgimage->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, orgimage->data);

        shaderProgram->bind();
        shaderProgram->setUniformValue("zoomCenter", zoomCenter);
        shaderProgram->setUniformValue("zoomRatio", zoomRatio);
        renderQuad();
        shaderProgram->release();

    } // Render by GlBufferData
    else if (!pautmgr->isGLTexture() && pautmgr->prosdt.sviewID) {
        // Clear texture (optional, if needed)
        glBindTexture(GL_TEXTURE_2D, pautmgr->prosdt.sviewID);
        float clearColor[4] = { 0.2f, 0.0f, 0.0f, 0.0f };
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pautmgr->prosdt.ysize, pautmgr->prosdt.zsize, GL_RGBA, GL_FLOAT, clearColor);
        glBindTexture(GL_TEXTURE_2D, 0);    

        // Render texture
        shaderProgram->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pautmgr->prosdt.sviewID);
        shaderProgram->setUniformValue("u_Texture", 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
        shaderProgram->release();
    }
    glFlush();
    if (float avgFrameTime = EndFrameTimer(frameTracker); avgFrameTime > 0) { ReadStatus::getinstance().set_sviewfps(avgFrameTime + 0.01); }
}
void SviewFrame::renderQuad() {
    static const GLfloat vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
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
}
void SviewFrame::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

bool isDragging = false;
QPointF lastMousePos;
void SviewFrame::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isSelectingRegion = true;
        selectionStart = event->pos();
        selectionEnd = event->pos();
    }
    else if (event->button() == Qt::RightButton) {
        isDragging = true;
        lastMousePos = event->pos();
    }
}
void SviewFrame::mouseMoveEvent(QMouseEvent* event) {
    if (isSelectingRegion) {
        selectionEnd = event->pos();
        update();
    }
    else if (isDragging) {
        QPointF currentMousePos = event->pos();
        QPointF delta = currentMousePos - lastMousePos;
        zoomCenter.setX(zoomCenter.x() - delta.x() / this->width());
        zoomCenter.setY(zoomCenter.y() - delta.y() / this->height());
        zoomCenter.setX(std::clamp(zoomCenter.x(), 0.0f, 1.0f));
        zoomCenter.setY(std::clamp(zoomCenter.y(), 0.0f, 1.0f));
        lastMousePos = currentMousePos;
        update();
    }
}
void SviewFrame::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isSelectingRegion) {
        isSelectingRegion = false;
        QRectF selectionRect = QRectF(selectionStart, selectionEnd).normalized();
        if (selectionRect.width() > 5 && selectionRect.height() > 5) {
            float centerX = (selectionRect.x() + selectionRect.width() / 2.0f) / this->width();
            float centerY = (selectionRect.y() + selectionRect.height() / 2.0f) / this->height();
            centerY = 1.0f - centerY;
            float xRatio = (float)this->width() / (float)selectionRect.width();
            float yRatio = (float)this->height() / (float)selectionRect.height();
            float newZoomRatio = std::min(xRatio, yRatio);
            zoomCenter = QVector2D(centerX, centerY);
            zoomRatio = std::clamp(1.0f / newZoomRatio, 0.1f, 20.0f);
            update();
        }
        selectionStart = QPointF();
        selectionEnd = QPointF();
    }
    else if (event->button() == Qt::RightButton) {
        isDragging = false;
    }
}
void SviewFrame::wheelEvent(QWheelEvent* event) {
    QPointF mousePos = event->position();
    float x = static_cast<float>(mousePos.x()) / this->width();
    float y = static_cast<float>(mousePos.y()) / this->height();
    y = 1.0f - y;
    zoomCenter = QVector2D(x, y);
    float delta = event->angleDelta().y();
    float zoomStep = 0.05f;
    if (delta > 0) {
        zoomRatio += zoomStep;
    }
    else {
        zoomRatio -= zoomStep;
    }
    if (zoomRatio > 1.0f) zoomRatio = 1.0f;
    zoomRatio = std::clamp(zoomRatio, 0.1f, 5.0f);
    update();
}
void SviewFrame::updateRealTime() {
    if (!isRealTime) {
        if (graphicsView) {
            layout->removeWidget(graphicsView.get());
            graphicsView->hide();
        }
        layout->addWidget(this);
        this->show();
        isRealTime = true;
    }
}
void SviewFrame::paintEvent(QPaintEvent* event) {
    QOpenGLWidget::paintEvent(event);
    if (isSelectingRegion) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(Qt::DashLine);
        painter.setBrush(Qt::NoBrush);
        QRectF selectionRect = QRectF(selectionStart, selectionEnd).normalized();
        painter.drawRect(selectionRect);
    }
}


// ********************************************
// Offline Reading
// TODO: Optimize into GPU Rendering
void SviewFrame::updateOffLine() {
    if (isRealTime)
    {
        layout->removeWidget(this);
        this->hide();
        layout->addWidget(graphicsView.get());
        graphicsView->show();
        isRealTime = false;
    }

    if (scandat.Amplitudes.empty()) return;
    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(zsize, ysize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

#pragma omp parallel for
    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + curpt[0];
            if (index >= scandat.Amplitudes.size()) {
                sttlogs->logWarning("[Bscan] Out of range data: " + std::to_string(index));
                return;
            }
            double percentAmplitude = std::abs(scandat.Amplitudes[index]) / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];
            orgimage->at<cv::Vec3b>(z, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(orgimage->cols) / static_cast<double>(orgimage->rows);

    int newWidth = (frameRatio > imageRatio) ? static_cast<int>(orgimage->rows * frameRatio) : orgimage->cols;
    int newHeight = (frameRatio > imageRatio) ? orgimage->rows : static_cast<int>(orgimage->cols / frameRatio);

    int scaleFactor = (!curpt || ConfigL->settingconf->bhighResBscan) ? ConfigL->sysParams->resolution : 1;
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

    auto artWork = new QGraphicsItemGroup();
    scene->addItem(artWork);
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
    pixmapItem->setData(0, "artwork");
    artWork->addToGroup(pixmapItem);

    //graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    graphicsView->update();


    addPoints(true, -1, -1);
}
void SviewFrame::addPoints(bool Cviewlink, int x, int y)
{
    double pixelY, pixelZ;
    if (!isRealTime)
    {
        pixelY = (Cviewlink) ? static_cast<double>(curpt[1]) * scaledImage->cols / ysize : static_cast<double>(x);
        pixelZ = (Cviewlink) ? static_cast<double>(curpt[1]) * scaledImage->rows / zsize : static_cast<double>(y);
    }
    else {
        pixelY = (Cviewlink) ? static_cast<double>(oms.OMS->beamCurrentID) * scaledImage->cols / ysize : static_cast<double>(x);
        pixelZ = (Cviewlink) ? static_cast<double>(curpt[2]) * scaledImage->rows / zsize : static_cast<double>(y);
    }
    if (overlay) overlay->updatePoints(pixelY, pixelZ, Qt::red, Qt::color0);
    graphicsView->update();
}
std::pair<int, int> SviewFrame::calculateOriginalPos(int scaled_y, int scaled_z) {
    if (scaledImage == nullptr) throw std::exception();
    int original_y; int original_z;
    if (!isRealTime)
    {
        original_y = static_cast<int>(scaled_y * static_cast<double>(orgimage->cols) / scaledImage->cols);
        original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);
        if (original_y < 0 || original_z < 0 || original_z >= zsize || original_y >= ysize) throw std::exception();
    }
    else
    {
		original_y = static_cast<int>(scaled_y * static_cast<double>(orgimage->cols) / scaledImage->cols);
        if (original_y < 0 || original_y > oms.OMS->beamNumber)
            original_y = 0;
		//int original_z = static_cast<int>(scaled_z * static_cast<double>(orgimage->rows) / scaledImage->rows);
        original_z = scaled_z;
    }
    return { original_y, original_z };
}
void SviewFrame::MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView)
{
    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseMoved, [=](int scaled_y, int scaled_z) {
        try
        {
            if (!isRealTime)
            {
                auto [original_y, original_z] = calculateOriginalPos(scaled_y, scaled_z);
                QToolTip::showText(QCursor::pos(), QString("X: %1\nY: %2\nZ: %3").arg(curpt[0]).arg(original_y).arg(original_z));
                overlay->updateOverlay(scaled_y, scaled_z, scaledImage->cols, scaledImage->rows);
                graphicsView->update();
            }
            else
            {
                int newy;
                //std::tie(newy, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
                if (!scaledImage) return;                
                QString tooltipText = QString("X: %1\nY: %2\nZ: %3")
                    .arg("...")
                    .arg(scaled_y != -1 ? QString::number(scaled_y) : "...")
                    .arg(scaled_z != -1 ? QString::number(scaled_z) : "...");

                QToolTip::showText(QCursor::pos(), tooltipText);
                overlay->updateOverlay(scaled_y, scaled_z, scaledImage->cols, scaledImage->rows);
            }
        }
        catch (...) { (void)0; }
        });

    QObject::connect(graphicsView.get(), &ZoomableGraphicsView::mouseClicked, [=](int scaled_y, int scaled_z) {
        try
        {
            if (!isRealTime)
            {
                std::tie(curpt[1], curpt[2]) = calculateOriginalPos(scaled_y, scaled_z);
                isPanning = false;
                addPoints(false, scaled_y, scaled_z);

            }
            else
            {
                std::tie(oms.OMS->beamCurrentID, curpt[2]) = calculateOriginalPos(scaled_y, scaled_z);
                isPanning = false;
                addPoints(false, scaled_y, scaled_z);
            }
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


