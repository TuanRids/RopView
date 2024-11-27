#include "SviewFrame.h"
#include <omp.h>
#include "SystemConfig/ConfigLocator.h"

SviewFrame::SviewFrame(QWidget* parent)
    : QOpenGLWidget(parent), surface(new QOffscreenSurface), scaleXPtr(nullptr), scaleYPtr(nullptr)
{
    QOpenGLWidget::setUpdateBehavior(QOpenGLWidget::PartialUpdate);
    // connect(this, &SviewFrame::aboutToCompose, this, &SviewFrame::initGLSlot);
    //connect(this, &SviewFrame::frameSwapped, this, &SviewFrame::paintGLSlot);
}

QWidget* SviewFrame::createFrame() {
    if (!graphicsView) {
        graphicsView = std::make_shared<QGraphicsView>();
    }
    if (!scene) {
        scene = std::make_shared<QGraphicsScene>();
    }
    graphicsView->setScene(scene.get());
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* frame = new QWidget();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    //auto* openglwidget = new QOpenGLWidget(graphicsView.get());
	//layout->addWidget(openglwidget);
    
//    connect(openglwidget, &QOpenGLWidget::aboutToCompose, this, &SviewFrame::initializeGL);

    layout->addWidget(this);
    frame->setLayout(layout);
    return frame;
}

void SviewFrame::initializeGL() {   
    if (!shaderProgram) {
        sttlogs->logNotify("Start initialize OpenGL on GPU for SviewFrame");
        initializeOpenGLFunctions();
        
        std::cout << "OpenGL Context: " << context() << std::endl;
        if (!context()->isValid()) {
            sttlogs->logCritical("OpenGL context is not valid");
        }

        // Set clear color (background red)
        glClearColor(0.5f, 0.4f, 0.0f, 1.0f);

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
                gl_Position = vec4(position.x * u_Scale.x, position.y * u_Scale.y , 0.0, 1.0);
                vertexColor = color;
            }
            )");
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Geometry,
            R"(
            #version 330 core
            layout(points) in;
            layout(points, max_vertices = 100) out;

            in vec3 vertexColor[]; 
            out vec3 fragColor;   

            uniform int u_Density; 

            void main() {
                float step = 2.0 / (u_Density * gl_in.length());

                for (int i = 0; i < u_Density; ++i) {
                    vec4 newPos = gl_in[0].gl_Position + vec4(step * i, 0.0, 0.0, 0.0); 
                    gl_Position = newPos;   
                    fragColor = vertexColor[0]; 
                    EmitVertex();
                }
                EndPrimitive();

            }
            )");

        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            R"(
            #version 330 core
            in vec3 fragColor; 
            out vec4 FragColor;

            void main() {
                FragColor = vec4(fragColor, 1.0);
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
        vbo.allocate(vertice_sview.constData(), vertice_sview.size() * sizeof(VertexData));

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

void SviewFrame::paintGL() {
    // TODO if zoom: only draw in the visible area for faster rendering


    static QOpenGLContext* context = QOpenGLContext::currentContext();
    if (!context) {
        sttlogs->logCritical("OpenGL context is not valid");
        return;
    }
    

    // glViewport(0, 0, width(), height());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(5.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shaderProgram->bind();

    shaderProgram->setUniformValue("u_Scale", QVector2D(1, 1));
    shaderProgram->setUniformValue("u_Density", 3);

    if (!vertice_sview.isEmpty()) {

        static QElapsedTimer fpsTimer;
        static int frameCount = 0;
        if (!fpsTimer.isValid()) { fpsTimer.start(); }
        if (fpsTimer.elapsed() >= 1000) {
            float avgEachFrameTime = fpsTimer.elapsed() / static_cast<float>(frameCount);
            ReadStatus::getinstance().set_sviewfps(avgEachFrameTime);
            fpsTimer.restart();
            frameCount = 0;
#ifdef _DEBUG
            std::cout << "size: " << vertice_sview.size() << std::endl;
#endif
        }
        frameCount++;
        
        vao.bind();
        vbo.bind();

        // Update vertex buffer data
        size_t dataSize = vertice_sview.size() * sizeof(VertexData);
        glBufferData(GL_ARRAY_BUFFER, dataSize, vertice_sview.constData(), GL_DYNAMIC_DRAW);

#ifdef _DEBUG
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) { qDebug() << "GL error: " << err ; }
#endif
        // Draw points
        glDrawArrays(GL_POINTS, 0, vertice_sview.size());

        vao.release();
        vbo.release();
    }

    shaderProgram->release();
    glFlush();
    glFinish();
}

void SviewFrame::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    std::cout << "Size: " << w << "x" << h << std::endl;
}

void SviewFrame::updateRealTime()
{    


    //makeCurrent();
    //paintGL();     
    //doneCurrent(); 
    //emit frameSwapped();
    //context()->swapBuffers(context()->surface());
    /*
    try {
        if (!isRealTime) { scene->clear(); isRealTime = true; }
        orgimage = std::make_shared<cv::Mat>(ArtScan->SViewBuf->clone());
        if (!orgimage) return;
        for (auto item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                scene->removeItem(item);
                delete item;
                break;
            }
        }
        auto ScaleRatio = orgimage->rows/graphicsView->size().height();
        int frameWidth = graphicsView->size().width() * ScaleRatio;
        int frameHeight = graphicsView->size().height() * ScaleRatio;

        if (ConfigL->visualConfig->setPautMode == PautModeOmni::Linear)
        {
            // MXU is scaled
            scaledImage = std::make_unique<cv::Mat>();
            cv::resize(*orgimage, *scaledImage, cv::Size(frameWidth, frameHeight), 0, 0, cv::INTER_LINEAR);//INTER_NEAREST
            //scaledImage = orgimage;
        }
        else if (ConfigL->visualConfig->setPautMode == PautModeOmni::Sectorial)
        {
            scaledImage = std::make_unique<cv::Mat>();
            cv::resize(*orgimage, *scaledImage, cv::Size(frameWidth, frameHeight), 0, 0, cv::INTER_LINEAR);//INTER_NEAREST
            //scaledImage = orgimage;
        }
        //cv::GaussianBlur(*scaledImage, *scaledImage, cv::Size(1, 1), cv::BORDER_CONSTANT);

        auto qImage = std::make_shared<QImage>(scaledImage->data, scaledImage->cols, scaledImage->rows, scaledImage->step, QImage::Format_RGB888);

        *qImage = qImage->rgbSwapped();  
        QPixmap pixmap = QPixmap::fromImage(*qImage);

        auto pixmapItem = scene->addPixmap(pixmap);
        pixmapItem->setZValue(-1);
        pixmapItem->setData(0, "artwork");
                
        graphicsView->update();
    }
	catch (exception& e)
	{ std::cout << e.what() << std::endl; return; } 
    */
}



void SviewFrame::updateOffLine() {
    isRealTime = false;
    if (scandat.Amplitudes.empty()) return;
    zsize = scandat.AmplitudeAxes[0].Quantity;
    ysize = scandat.AmplitudeAxes[1].Quantity;
    xsize = scandat.AmplitudeAxes[2].Quantity;

    orgimage = std::make_unique<cv::Mat>(zsize, ysize, CV_8UC3);
    scaledImage = std::make_unique<cv::Mat>();

    auto everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
#pragma omp parallel for
    for (uint64_t z = 0; z < zsize; ++z) {
        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z * (xsize * ysize) + y * xsize + curpt.x;
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

    int scaleFactor = (!isPanning || ConfigL->settingconf->bhighResBscan) ? ConfigL->sysParams->resolution : 1;
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
        pixelY = (Cviewlink) ? static_cast<double>(curpt.y) * scaledImage->cols / ysize : static_cast<double>(x);
        pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);
    }
    else {
        pixelY = (Cviewlink) ? static_cast<double>(oms.OMS->beamCurrentID) * scaledImage->cols / ysize : static_cast<double>(x);
        pixelZ = (Cviewlink) ? static_cast<double>(curpt.z) * scaledImage->rows / zsize : static_cast<double>(y);
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
                QToolTip::showText(QCursor::pos(), QString("X: %1\nY: %2\nZ: %3").arg(curpt.x).arg(original_y).arg(original_z));
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
                std::tie(curpt.y, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
                isPanning = false;
                addPoints(false, scaled_y, scaled_z);

            }
            else
            {
                std::tie(oms.OMS->beamCurrentID, curpt.z) = calculateOriginalPos(scaled_y, scaled_z);
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


