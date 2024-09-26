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

struct BufIN {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkBuffer idbuffer = VK_NULL_HANDLE;      
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceMemory indexmemory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo uniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDeviceSize vertexAllocSize;
    VkDeviceSize indexAllocSize;
    VkDeviceSize uniformAllocSize;
};
struct DesIN {
    VkDescriptorPool descPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout descSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet descSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
};

struct PipeIN {
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
};
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

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexMemory = VK_NULL_HANDLE;
    VkBuffer idbuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexMemory = VK_NULL_HANDLE;
};

class ResourceBuilder {
public:
    ResourceBuilder(QVulkanDeviceFunctions* devFuncs, VkDevice device, std::shared_ptr<BufIN> gbufin, std::shared_ptr<DesIN> gdesin, std::shared_ptr<PipeIN> gpipein)
        : m_deviFunc(devFuncs), m_device(device), bufin(gbufin), desin(gdesin), pipein(gpipein) {}


    ResourceBuilder& createBuffer(QVulkanWindow* m_VulWindow, Mesh& gmesh);
    ResourceBuilder& allocateMemory(QVulkanWindow* m_VulWindow, Mesh& gmesh);
    ResourceBuilder& createDescriptor();
    ResourceBuilder& createPipeLine(QVulkanWindow* m_VulWindow);

private:
    VkShaderModule createShader(const QString& name);
    QVulkanDeviceFunctions* m_deviFunc;
    VkDevice m_device;
    VkDeviceSize vertexAllocSize;
    VkDeviceSize uniformAllocSize;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    static int concurrentFrameCount;
    
    std::shared_ptr<BufIN> bufin;
    std::shared_ptr<DesIN> desin;
    std::shared_ptr<PipeIN> pipein;

    VkResult result;
};

#endif // RESOURCE_BUILDER_H
