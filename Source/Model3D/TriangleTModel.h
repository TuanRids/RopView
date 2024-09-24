#ifndef MODELDATATYPE_H
#define MODELDATATYPE_H

#include "..\pch.h"
#include <vector>
#include <cmath>
#include <QWidget>
#include <QVulkanWindow>
#include "..\MainUI\statuslogs.h"
QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)
QT_FORWARD_DECLARE_CLASS(QLCDNumber)

// Define the structure for storing vertex data

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

protected:
    // ========= Shader Creation =========
    VkShaderModule createShader(const QString& name);

    // ========= Vulkan Variables =========
    QVulkanWindow* m_window;
    QVulkanDeviceFunctions* m_devFuncs;

    // ========= Buffer and Memory Management =========
    VkDeviceMemory m_bufMem = VK_NULL_HANDLE;
    VkBuffer m_buf = VK_NULL_HANDLE;
    VkDescriptorBufferInfo m_uniformBufInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    // ========= Descriptor and Pipeline Layouts =========
    VkDescriptorPool m_descPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet m_descSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    // ========= Pipeline Management =========
    VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;

    // ========= Transformation and State Variables =========
    QMatrix4x4 m_proj;
    float m_rotation = 0.0f;

    // ========= logs and others =========   
    nmainUI::statuslogs* sttlogs;
};

#endif
