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
    if (!shaderProgram) {
        sttlogs->logNotify("Start initialize OpenGL on GPU for CviewFrame");
        initializeOpenGLFunctions();

        std::cout << "OpenGL Context: " << context() << std::endl;
        if (!context()->isValid()) {
            sttlogs->logCritical("OpenGL context is not valid");
        }

        // Set clear color (background blue)
        glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

        // Create shader program
        shaderProgram = std::make_unique<QOpenGLShaderProgram>();
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            R"(
        #version 330 core
        layout (location = 0) in vec2 position;
        layout (location = 1) in vec3 color;

        uniform vec2 u_Scale;
        out vec3 vertexColor;

        void main() {
            gl_Position = vec4(position.x * u_Scale.x, position.y * u_Scale.y, 0.0, 1.0);
            vertexColor = color;
        }
        )");
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            R"(
        #version 330 core
        in vec3 vertexColor; 
        out vec4 FragColor;

        void main() {
            FragColor = vec4(vertexColor, 1.0);
        }
        )");
        shaderProgram->link();
        shaderProgram->bind();

        // Configure vertex and color attributes
        int vertexLocation = shaderProgram->attributeLocation("position");
        int colorLocation = shaderProgram->attributeLocation("color");

        vao.create();
        vao.bind();

        vbo.create();
        vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        vbo.bind();
        vbo.allocate(vertice_cview.constData(), vertice_cview.size() * sizeof(VertexData));

        glEnableVertexAttribArray(vertexLocation);
        glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(offsetof(VertexData, position)));

        glEnableVertexAttribArray(colorLocation);
        glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(offsetof(VertexData, color)));

        vao.release();
        vbo.release();
        shaderProgram->release();
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

#ifdef _DEBUG
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) { qDebug() << "GL error: " << err; }
#endif
        // Draw points
        glDrawArrays(GL_POINTS, 0, vertice_cview.size());

        vao.release();
        vbo.release();
    }

    shaderProgram->release();
    glFlush();
    glFinish();
}
void CviewFrame::updateRealTime()
{
    return;
    if (!isRealTime) {
        scene->clear();
        isRealTime = true;
    }
    orgimage = std::make_shared<cv::Mat>(ArtScan->CViewBuf->clone());
    if (!orgimage) return;

    scaledImage = std::make_unique<cv::Mat>();

    int originalWidth = orgimage->cols;
    int newWidth = graphicsView->size().width();

    int newHeight = graphicsView->size().height();

    cv::resize(*orgimage, *scaledImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);
    cv::flip(*scaledImage, *scaledImage, 1);
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

    graphicsView->update();
}
void CviewFrame::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
#ifndef _DEBUG
    std::cout << "Cview ReSize: " << width << "x" << height << std::endl;
#endif
}


void CviewFrame::updateOffLine() {
    isRealTime = false;
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

    std::vector<Color> everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
    uint64_t z_offset = curpt.z * (xsize * ysize);
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
    double pixelX= (Cviewlink) ? static_cast<double>(curpt.x) * scaledImage->cols / xsize : static_cast<double>(x);
    double pixelY = (Cviewlink) ? static_cast<double>(curpt.y) * scaledImage->rows / ysize : static_cast<double>(y);

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
            QString tooltipText = QString("X: %1\nY: %2\nZ: %3").arg(original_x).arg(original_y).arg(curpt.z);
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
            std::tie(curpt.x, curpt.y) = calculateOriginalPos(scaled_x, scaled_y);
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