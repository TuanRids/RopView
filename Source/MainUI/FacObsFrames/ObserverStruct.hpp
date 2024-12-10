#ifndef OBSERVER_STRUCT_H
#define OBSERVER_STRUCT_H
#include "../pch.h"
#include "SystemConfig/ConfigLocator.h"
#include "Instrumentation/IAScanCollection.h"

struct VertexData {
	QVector2D position;
	QVector3D color;
};

struct ProcessingContext {
    std::atomic<bool> nIsGlTexture = true;
    // Realtime processing parameters UpdateCPU/GPU
    static constexpr uint16_t BCsize = 1000;
    int zsize = -1;
    int ysize = -1;
    double angStart;
    double angEnd;
    PautModeOmni pautmode;
    int sview_x = -1;
    double minAmplitude = 1000.0;
    double maxAmplitudeSampling = -1.0;
    double maxAmplitudeUsable = -1.0;

    // Realtime processing state
    GLuint sectorialBuffer = 0;
    deque<std::shared_ptr<IAscanCollection>> nAscanCollection;
    UIArtScan* ArtScan = nullptr;

    QVector<VertexData> vertice_sview;
    QVector<VertexData> vertice_cview;
    std::atomic<bool> isGlTexture = false;
    bool isPanning = false;
    int curpt[3] = { 0, 0, 0 };
    AscanData scandat;

    std::vector<std::vector<cv::Point>> xySectorial;

    ProcessingContext()
        : angStart(OmSetupL::getInstance().OMS->BeamStartAngle),
        angEnd(OmSetupL::getInstance().OMS->beamNumber + OmSetupL::getInstance().OMS->BeamStartAngle),
        pautmode(ConfigLocator::getInstance().visualConfig->setPautMode) {}
};


#endif