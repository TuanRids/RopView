#ifndef CVIEWFRAME_H
#define CVIEWFRAME_H

#include "..\pch.h"
#include "PAUTFileReader/AscanProcessor.h"
#include "MainUI/FacObsFrames/FactoryMgr.h"
#include "MainUI/FacObsFrames/ObserverMgr.h"
#include "MainUI/mainwindow.h"
#include "event/ZoomableGraphicsView.h"
#include "event/XYOverlayGrid.h"


// Graphics Frame
class CviewFrame : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core, public nObserver {
private:
    // Functions
    void CreateXYview();
    void MouseGetPosXY();
    void addPoints(bool Cviewlink, int x, int y);
    std::pair<int, int> calculateOriginalPos(int scaled_y, int scaled_z);
    // View Variables
    std::shared_ptr<QGraphicsScene> scene;
    std::shared_ptr<ZoomableGraphicsView> graphicsView;
    std::shared_ptr<XYOverlayGrid> overlay = nullptr;
    bool isRealTime = false;
    // Offline Variables
    uint64_t ysize, xsize, zsize;
    std::shared_ptr<cv::Mat> orgimage;
    std::shared_ptr<cv::Mat> scaledImage;
    // RealtimeGPU Variables
    QVBoxLayout* layout;
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram = nullptr;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOffscreenSurface* surface;

    // Realtime GPU variables for GLTexture
    QOpenGLBuffer ebo;
    GLuint textureID;

public:
    explicit CviewFrame(QWidget* parent = nullptr);
    QWidget* createFrame() override;
    void updateOffLine() override;
    void updateRealTime() override;
    void setter_Curpt(int x, int y, int z) {curpt.x = x; curpt.y = y; curpt.z = z;}
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
};

#endif
