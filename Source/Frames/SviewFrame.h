#ifndef SSCANFRAME_H
#define SSCANFRAME_H

#include "../pch.h"
#include "PAUTFileReader/AscanProcessor.h"
#include "MainUI/FacObsFrames/FactoryMgr.h"
#include "MainUI/FacObsFrames/ObserverMgr.h"
#include "MainUI/mainwindow.h"
#include "event/ZoomableGraphicsView.h"
#include "event/XYOverlayGrid.h"

// Graphics Frame
class SviewFrame : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core, public nObserver {
    Q_OBJECT
private:
    // Functions
    void MouseGetPosXY(std::shared_ptr<ZoomableGraphicsView> graphicsView);
    void addPoints(bool Cviewlink, int x, int y);
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);
    // View Variables
    std::shared_ptr<XYOverlayGrid> overlay = nullptr;
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    // Offline Variables
    uint64_t ysize, xsize, zsize;
    std::shared_ptr<cv::Mat> orgimage;
    std::shared_ptr<cv::Mat> scaledImage;
    bool isRealTime = true;
    // RealtimeGPU Variables
    QVBoxLayout* layout;
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram = nullptr;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOffscreenSurface* surface;

public:
    explicit SviewFrame(QWidget* parent = nullptr);
    QWidget* createFrame() override;
    void updateOffLine() override;
    void updateRealTime() override;
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
};



#endif
