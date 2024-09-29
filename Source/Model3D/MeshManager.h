#ifndef MODELDATATYPE_H
#define MODELDATATYPE_H

#include "..\pch.h"
#include <vector>
#include <cmath>
#include <QWidget>
#include <QVulkanWindow>
#include "..\MainUI\statuslogs.h"
#include "ResourceBuilder.h"

class MeshRenderer : public QVulkanWindowRenderer
{
public:
    // ========= Constructor =========
    MeshRenderer(QVulkanWindow* w, bool msaa = false);

    // ========= Main Rendering Methods =========
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void setupRenderEnvironment(VkCommandBuffer cb, const QSize& sz);

    void renderMesh(VkCommandBuffer cb);
    void wheelZoom(float zoom_level);
    // ========= Adjust for next frame =========
    void applyRotation(QMatrix4x4& m);
    void updateRotation(float deltaX, float deltaY);
    void startNextFrame() override;
protected:
     
    // ========= Vulkan Variables =========
    QVulkanWindow* m_VulWindow;
    QVulkanDeviceFunctions* m_deviFunc;

    // ========= Transformation and State Variables =========
    QMatrix4x4 m_proj;

    float m_rotationX = 0.0f;
    float m_rotationY = 0.0f;
    int m_rotation = 0;
    float m_zoom_level = 0.0f;
    std::vector<std::shared_ptr<Mesh>> omesh;
    VkSwapchainKHR m_swapchain;
    std::unique_ptr<ResourceBuilder> upResBuilder;
    // ========= logs and others =========   
    nmainUI::statuslogs* pSttLogs;
};

#endif
