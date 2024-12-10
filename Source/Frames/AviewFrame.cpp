#include "AviewFrame.h"
#include "..\pch.h"
#include <omp.h>
#include <random>
#include "SystemConfig/ConfigLocator.h"

int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
AviewFrame::AviewFrame(QWidget* parent) : QOpenGLWidget(parent), surface(new QOffscreenSurface)
{
    QOpenGLWidget::setUpdateBehavior(QOpenGLWidget::PartialUpdate);
}

QWidget* AviewFrame::createFrame() {
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
void AviewFrame::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.09f, 0.09f, 0.09f, 1.0f);
    if (!shaderProgram) {
        QOffscreenSurface ggsurface;
        ggsurface.create();

        QOpenGLContext ggcontext;
        ggcontext.create();
        QString gpuVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        QString gpuRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        sttlogs->logNotify("GPU Vendor: " + gpuVendor.toStdString() + "\nGPU Renderer: " + gpuRenderer.toStdString());
        sttlogs->logInfo("Start initialize OpenGL - GLDataBuffer on GPU for AviewFrame");

        shaderProgram = std::make_unique<QOpenGLShaderProgram>();
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            R"(
            #version 430 core
            layout (location = 0) in vec2 position;

            void main() {
                gl_Position = vec4(position.x, -position.y, 0.0, 1.0);
            }
            )");
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            R"(
            #version 430 core
            out vec4 FragColor;

            void main() {
                FragColor = vec4(1.0, 0.5, 0.0, 1.0); // White line
            }
            )");
        shaderProgram->link();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_LINE_SMOOTH); // Enable line smoothing
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Hint to make smoothing as nice as possible
        // Create buffers
        vao.create();
        vao.bind();

        vbo.create();
        vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        vbo.bind();

        // Set vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

        vao.release();
        vbo.release();
        glClearColor(0.09f, 0.09f, 0.09f, 1.0f);

    }
    
    connect(this, &QOpenGLWidget::frameSwapped, this, [&]() {
        QOpenGLWidget::update();
        });
    QOpenGLWidget::update();
}
void AviewFrame::paintGL() {
    static QOpenGLContext* context = QOpenGLContext::currentContext();
    if (!context) {
        sttlogs->logCritical("OpenGL context is not valid");
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram->bind();
    if (!nIsGlTexture)
    {
        return;
    }
    if (ArtScan->AViewBuf && !ArtScan->AViewBuf->size() < 1) {
        vao.bind();
        vbo.bind();

        // Update VBO with current data
        glBufferData(GL_ARRAY_BUFFER, ArtScan->AViewBuf->size() * sizeof(glm::vec2), ArtScan->AViewBuf->data(), GL_DYNAMIC_DRAW);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            qDebug() << "OpenGL Error:" << error;
        }

        // Draw line strip
        glDrawArrays(GL_LINE_STRIP, 0, ArtScan->AViewBuf->size());
        error = glGetError();
        if (error != GL_NO_ERROR) {
            qDebug() << "OpenGL Error:" << error;
        }

        vao.release();
        vbo.release();
    }

    shaderProgram->release();

}

void AviewFrame::updateRealTime()
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
void AviewFrame::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "Aview ReSize: " << width << "x" << height << std::endl;
}

// ********************************************
// Offline Reading
// TODO: Optimize into GPU Rendering
void AviewFrame::updateOffLine()
{
    if (isRealTime)
    {
        layout->removeWidget(this);
        this->hide();
        layout->addWidget(graphicsView.get());
        graphicsView->show();
        isRealTime = false;
    }    static bool first_flag = false;
    // ********** PARAMETER VALIDATION **********
    if (scandat.Amplitudes.empty()) {
        if (sttlogs) {
            sttlogs->logWarning("No amplitude data available");
        }
        return;
    }

    // ********** PARAMETER PROCESSING **********
    if (!lineSeries) RenderFrame();

    uint64_t zsize = scandat.AmplitudeAxes[0].Quantity;
    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    points.clear();
    points.reserve(zsize);

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();

    for (uint64_t z = 0; z < zsize; ++z) {
        uint64_t index = z * (xsize * ysize) + curpt[1] * xsize + curpt[0];

        if (index >= scandat.Amplitudes.size()) {
            sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
            return;
        }

        int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
        double percentAmplitude = samplingAmplitude / (32768 / 100.0);

        minY = std::min(minY, percentAmplitude);
        maxY = std::max(maxY, percentAmplitude);
        points.append(QPointF(percentAmplitude, z));
    }
    if (!lineSeries) RenderFrame();
    lineSeries->clear();
    lineSeries->replace(points);
    if (!axisX || !first_flag)
    {
        first_flag = true;
        axisX->setRange(minY, 100);
        axisY->setRange(0, zsize);
        axisY->setReverse(true);
    }

    if (!isPanning)
    {
        static int lastpos[3] = { curpt[0], curpt[1], curpt[2]};
        if (curpt[0] != lastpos[0] || curpt[1] != lastpos[1] || curpt[2] != lastpos[2])
        {
            lastpos[0] = curpt[0]; lastpos[1] = curpt[1]; lastpos[2] = curpt[2];
            sttlogs->logInfo("Coord x: " + std::to_string(curpt[0]) + " - Coord y: " + std::to_string(curpt[1]) + " Coord z: " + std::to_string(curpt[2]) + ".");
        }
    }
}
void AviewFrame::RenderFrame()
{
    // ********** INITIALIZATION & SETTINGS **********

    static QFont axisFont;
    static QPen linePen(QColor(0, 102, 204));

    if (!chart || !lineSeries) {
        axisX = new QValueAxis();
        axisY = new QValueAxis();       
        axisFont.setPointSize(8);
        linePen.setWidth(2);
        scene->clear();
        chart = new QChart();
        lineSeries = new QSplineSeries();

        // Attach series and axes
        chart->addSeries(lineSeries);
        chart->legend()->hide();
        chart->addAxis(axisX, Qt::AlignTop);
        chart->addAxis(axisY, Qt::AlignLeft);
        lineSeries->attachAxis(axisX);
        lineSeries->attachAxis(axisY);
        lineSeries->setPen(linePen);
        lineSeries->setPointsVisible(true);

        // Axis settings
        axisX->setTitleBrush(QBrush(QColor(Qt::darkCyan)));
        axisY->setTitleBrush(QBrush(QColor(Qt::darkCyan)));
        axisY->setLabelsColor(QColor(Qt::cyan));
        axisX->setLabelsFont(axisFont);
        axisY->setLabelsFont(axisFont);
        axisX->setLabelsColor(QColor(Qt::cyan));
        axisY->setLabelsColor(QColor(Qt::cyan));
        axisX->setGridLineColor(QColor(80, 80, 80));
        axisY->setGridLineColor(QColor(80, 80, 80));
        axisX->setLabelFormat("%02i");
        axisX->setTickCount(10);
        axisY->setTickCount(10);

        // Chart settings
        chart->setBackgroundBrush(Qt::NoBrush);
        chart->setMargins(QMargins(0, 0, 0, 0));
        chartView = new QChartView(chart);
        chartView->setStyleSheet("background: transparent");
        chartView->setAttribute(Qt::WA_TranslucentBackground);
        chartView->setRenderHint(QPainter::Antialiasing, true);
        chartView->setMinimumSize(1900, 1080);
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        chartView->setRubberBand(QChartView::RectangleRubberBand);
        chartView->setRenderHint(QPainter::Antialiasing, true);

        scene->addWidget(chartView);
        QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_R), chartView);
        QObject::connect(shortcut, &QShortcut::activated, [this]() {
            size_t dataSize = points.size();
            axisY->setRange(0, static_cast<int>(dataSize));
            axisX->setRange(0,100);
            int viewWidth = graphicsView->width() * 1.0;
            int viewHeight = graphicsView->height() * 1.0;
            double aspectRatio = static_cast<double>(viewWidth) / static_cast<double>(viewHeight);
            chartView->setFixedSize(viewWidth, viewHeight);
            chartView->setGeometry(-10, 10, viewWidth, viewHeight);
            graphicsView->update();
            });
    }
    // Adjust aspect ratio based on the graphicsView's size
    int viewWidth = graphicsView->width() * 1.0;
    int viewHeight = graphicsView->height() * 1.0;
    double aspectRatio = static_cast<double>(viewWidth) / static_cast<double>(viewHeight);
    chartView->setFixedSize(viewWidth, viewHeight);
    chartView->setGeometry(-10, 10, viewWidth, viewHeight);
    graphicsView->update();
}
