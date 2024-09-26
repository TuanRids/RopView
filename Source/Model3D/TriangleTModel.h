#ifndef MODELDATATYPE_H
#define MODELDATATYPE_H

#include "..\pch.h"
#include <vector>
#include <cmath>
#include <QWidget>
#include <QVulkanWindow>
#include "..\MainUI\statuslogs.h"
#include "ResourceBuilder.h"

class TriangleRenderer : public QVulkanWindowRenderer
{
public:
    // ========= Constructor =========
    TriangleRenderer(QVulkanWindow* w, bool msaa = false);

    // ========= Main Rendering Methods =========
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;
    void updateRotation(float deltaX, float deltaY);

protected:

    // ========= Vulkan Variables =========
    QVulkanWindow* m_VulWindow;
    QVulkanDeviceFunctions* m_deviFunc;

    // ========= Transformation and State Variables =========
    QMatrix4x4 m_proj;

    float m_rotationX = 0.0f;
    float m_rotationY = 0.0f;


    int m_rotation = 0;
    std::vector<Mesh> omesh;
    std::shared_ptr<BufIN> bufin;
    std::shared_ptr<DesIN> desin;
    std::shared_ptr<PipeIN> pipein;
    // ========= logs and others =========   
    nmainUI::statuslogs* pSttLogs;
};

#endif
