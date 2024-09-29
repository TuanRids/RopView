#ifndef RESOURCE_BUILDER_H
#define RESOURCE_BUILDER_H

#include "..\pch.h"

#include <QVulkanFunctions>
#include <stdexcept>
#include <vector>
#include <string>
#include <QWidget>
#include <cmath>
#include <QVulkanWindow>
#include "vulkan/vulkan.h"
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};


struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    glm::vec3 material;
    glm::mat4 transform;
    std::string name;

    VkDescriptorSet descSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorBufferInfo uniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDeviceSize vertexAllocSize;
    VkDeviceSize indexAllocSize;
    VkDeviceSize uniformAllocSize;

    VkBuffer buffer = VK_NULL_HANDLE;  
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDescriptorPool descPool = VK_NULL_HANDLE; 
    VkDescriptorSetLayout descSetLayout = VK_NULL_HANDLE; 

    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

};

class ResourceBuilder {
public:
    ResourceBuilder(QVulkanDeviceFunctions* devFuncs, VkDevice device)
        : m_deviFunc(devFuncs), m_device(device) {}


    ResourceBuilder& createBuffer(QVulkanWindow* m_VulWindow, Mesh& gmesh);
    ResourceBuilder& allocateMemory(QVulkanWindow* m_VulWindow, Mesh& gmesh);
    ResourceBuilder& createDescriptor(Mesh& gmesh);
    ResourceBuilder& createPipeLine(QVulkanWindow* m_VulWindow, Mesh& gmesh);
    inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign);
private:
    VkShaderModule createShader(const QString& name);
    QVulkanDeviceFunctions* m_deviFunc;
    VkDevice m_device;

    static int concurrentFrameCount;
    VkResult result;
};

#endif // RESOURCE_BUILDER_H
