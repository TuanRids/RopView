#include "MeshManager.h"
#include <random>

float randomFloat(float min = 0.01f, float max = 5.08f) {
    std::random_device rd;  // Random seed
    std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_real_distribution<float> dis(min, max); // Uniform distribution between min and max
    return dis(gen);
}

static uint16_t indices[] = {
    0, 1, 2,0,2,3 // First triangle
};

static constexpr int UNIFORM_DATA_SIZE = 16 * sizeof(float);


MeshRenderer::MeshRenderer(QVulkanWindow* w, bool msaa)
    : m_VulWindow(w)
{ 
    const int gridSize = 5;
    // test multiple objects
    unsigned int k = 0;
    for (k = 0; k < 5; ++k)
    {
        omesh.push_back(nullptr);
        omesh[k] = std::make_unique<Mesh>();
        for (int i = 0; i <= gridSize; ++i) {
            for (int j = 0; j <= gridSize; ++j) {
                float x = (float(i) / gridSize) - 0.2f;
                float y = (float(j) / gridSize) - 0.2f;
                float z = 0.1f * cosf(x * 3.1415f) * sinf(y * 3.1415f) + k*0.25f;
                if (k % 2 == 0) { z = -z; }
                omesh[k]->vertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0.3f + i * 0.1f, 0.4f + j * 0.1f, 0.5f)));
            }
        }
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                uint16_t topLeft = i * (gridSize + 1) + j;
                uint16_t topRight = topLeft + 1;
                uint16_t bottomLeft = (i + 1) * (gridSize + 1) + j;
                uint16_t bottomRight = bottomLeft + 1;

                omesh[k]->indices.insert(omesh[k]->indices.end(), {
                    topLeft, bottomLeft, topRight,
                    topRight, bottomLeft, bottomRight
                    });
            }
        }
    }
    omesh.push_back(nullptr);
    omesh[k] = std::make_unique<Mesh>();
    omesh[k]->vertices = {
        Vertex(glm::vec3(-0.6f,  0.2f,  0.2f), glm::vec3(0.2f, 0.6f, 0.3f)), // red
        Vertex(glm::vec3(-0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 0.3f, 1.0f)), // green
        Vertex(glm::vec3(-0.2f, -0.2f,  0.2f), glm::vec3(0.6f, 0.0f, 0.2f)), // blue
        Vertex(glm::vec3(-0.6f, -0.2f,  0.2f), glm::vec3(0.5f, 0.6f, 0.0f)), // yellow
        Vertex(glm::vec3(-0.6f,  0.2f, -0.2f), glm::vec3(0.4f, 0.2f, 0.2f)), // cyan
        Vertex(glm::vec3(-0.2f,  0.2f, -0.2f), glm::vec3(0.1f, 0.7f, 0.8f)), // magenta
        Vertex(glm::vec3(-0.2f, -0.2f, -0.2f), glm::vec3(0.6f, 0.1f, 0.4f)), // orange
        Vertex(glm::vec3(-0.6f, -0.2f, -0.2f), glm::vec3(0.2f, 0.4f, 0.6f))  // gray
    };
    omesh[k]->indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1,
        4, 5, 1, 1, 0, 4,
        3, 2, 6, 6, 7, 3
    };
    
    if (!pSttLogs) { pSttLogs = &nmainUI::statuslogs::getinstance(); }
    if (msaa) {
        const QList<int> counts = w->supportedSampleCounts();
        for (int s = 16; s >= 4; s /= 2) { if (counts.contains(s)) { m_VulWindow->setSampleCount(s); break; } }
    }
}

void MeshRenderer::initResources()
{
    m_deviFunc = m_VulWindow->vulkanInstance()->deviceFunctions(m_VulWindow->device());
        
    upResBuilder = std::make_unique<ResourceBuilder>(m_deviFunc, m_VulWindow->device());
    try {
        for (auto& mesh : omesh) {
            if (!mesh) { break; }
            upResBuilder->createBuffer(m_VulWindow, *mesh);
            upResBuilder->allocateMemory(m_VulWindow, *mesh);
            upResBuilder->createDescriptor(*mesh);
            upResBuilder->createPipeLine(m_VulWindow, *mesh);
        }
    }
    catch (std::exception& e) {
        qFatal("%s", e.what());
    }
}

void MeshRenderer::initSwapChainResources()
{
    qDebug("initSwapChainResources");
    releaseSwapChainResources();
    // Projection matrix
    m_proj = m_VulWindow->clipCorrectionMatrix(); // adjust for Vulkan-OpenGL clip space differences
    auto swapChainSize = m_VulWindow->swapChainImageSize();
    m_proj.perspective(45.0f, swapChainSize.width() / (float)swapChainSize.height(), 0.01f, 100.0f);
    m_proj.translate(0, 0, -4);
}

void MeshRenderer::releaseSwapChainResources()
{
    qDebug("releaseSwapChainResources");
}
void MeshRenderer::releaseResources()
{
    qDebug("releaseResources");

    VkDevice dev = m_VulWindow->device();
    m_deviFunc->vkDeviceWaitIdle(dev);

    
    for (auto& mesh : omesh) {
        if (!mesh) { break; }
        if (mesh->buffer) {
            m_deviFunc->vkDestroyBuffer(dev, mesh->buffer, nullptr);
            mesh->buffer = VK_NULL_HANDLE;
        }
        if (mesh->memory) {
            m_deviFunc->vkFreeMemory(dev, mesh->memory, nullptr);
            mesh->memory = VK_NULL_HANDLE;
        }
        if (mesh->pipeline) {
            m_deviFunc->vkDestroyPipeline(dev, mesh->pipeline, nullptr);
            mesh->pipeline = VK_NULL_HANDLE;
        }

        if (mesh->pipelineLayout) {
            m_deviFunc->vkDestroyPipelineLayout(dev, mesh->pipelineLayout, nullptr);
            mesh->pipelineLayout = VK_NULL_HANDLE;
        }

        if (mesh->pipelineCache) {
            m_deviFunc->vkDestroyPipelineCache(dev, mesh->pipelineCache, nullptr);
            mesh->pipelineCache = VK_NULL_HANDLE;
        }

        if (mesh->descSetLayout) {
            m_deviFunc->vkDestroyDescriptorSetLayout(dev, mesh->descSetLayout, nullptr);
            mesh->descSetLayout = VK_NULL_HANDLE;
        }

        if (mesh->descPool) {
            m_deviFunc->vkDestroyDescriptorPool(dev, mesh->descPool, nullptr);
            mesh->descPool = VK_NULL_HANDLE;
        }
    }
}

void MeshRenderer::startNextFrame() {
    VkDevice dev = m_VulWindow->device();
    VkCommandBuffer cb = m_VulWindow->currentCommandBuffer();
    const QSize sz = m_VulWindow->swapChainImageSize();

    // Set the viewport and scissor
    VkViewport viewport;
    viewport.x = viewport.y = 0;
    viewport.width = sz.width();
    viewport.height = sz.height();
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    // Setup rendering environment
    setupRenderEnvironment(cb, sz);

    for (const auto& mesh : omesh) {  
        quint8* p;
        VkResult err = m_deviFunc->vkMapMemory(dev, mesh->memory, mesh->uniformBufferInfo[m_VulWindow->currentFrame()].offset,
            UNIFORM_DATA_SIZE, 0, reinterpret_cast<void**>(&p));
        if (err != VK_SUCCESS) {
            qFatal("Failed to map memory: %d", err);
        }

        QMatrix4x4 m = m_proj;

        m_proj.setToIdentity();
        m_proj.perspective(45.0f, float(viewport.width) / viewport.height, 0.1f, 100.0f);
        m_proj.translate(0, 0, -4.0f * m_zoom_level);

        applyRotation(m);  

        memcpy(p, m.constData(), 16 * sizeof(float));  
        m_deviFunc->vkUnmapMemory(dev, mesh->memory);  
    }
    m_rotation++;  // Update the rotation angle

    m_deviFunc->vkCmdSetViewport(cb, 0, 1, &viewport);
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewport.width;
    scissor.extent.height = viewport.height;
    m_deviFunc->vkCmdSetScissor(cb, 0, 1, &scissor);
    // Render the mesh objects
    renderMesh(cb);

    


    m_deviFunc->vkCmdEndRenderPass(cb);

    m_VulWindow->frameReady();
    m_VulWindow->requestUpdate();
}

void MeshRenderer::updateRotation(float deltaX, float deltaY)
{
    m_rotationX += deltaX * 0.5f;  
    m_rotationY += deltaY * 0.5f;
}

void MeshRenderer::setupRenderEnvironment(VkCommandBuffer cb, const QSize& sz)
{
    VkClearColorValue clearColor = { { 0, 0, 0, 1 } };
    VkClearDepthStencilValue clearDS = { 1, 0 };
    VkClearValue clearValues[3];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = m_VulWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = m_VulWindow->currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = m_VulWindow->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;

    m_deviFunc->vkCmdBeginRenderPass(cb, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

}

void MeshRenderer::renderMesh(VkCommandBuffer cb)
{
    for (const auto& mesh : omesh) {
        if (!mesh) { break; }
        // Bind the pipeline and descriptor sets
        m_deviFunc->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh->pipeline);
        m_deviFunc->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh->pipelineLayout, 0, 1,
            &mesh->descSet[m_VulWindow->currentFrame()], 0, nullptr);

        // Bind vertex and index buffers
        VkDeviceSize vbOffset = 0;
        m_deviFunc->vkCmdBindVertexBuffers(cb, 0, 1, &mesh->buffer, &vbOffset);

        VkDeviceSize indexBufferOffset = upResBuilder->aligned(mesh->vertexAllocSize, m_VulWindow->physicalDeviceProperties()->limits.minUniformBufferOffsetAlignment);
        m_deviFunc->vkCmdBindIndexBuffer(cb, mesh->buffer, indexBufferOffset, VK_INDEX_TYPE_UINT16);

        // Draw indexed vertices
        m_deviFunc->vkCmdDrawIndexed(cb, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
    }
}

void MeshRenderer::wheelZoom(float zoom_level)
{
    m_zoom_level = zoom_level;
}

void MeshRenderer::applyRotation(QMatrix4x4& m) {
    // Logic to rotate the matrix (for example, around X and Y axis)
    m.rotate(m_rotationX, 0, 1, 0);  // Rotate on Y-axis
    m.rotate(m_rotationY, 1, 0, 0);  // Rotate on X-axis

    // You can add custom rotation logic here (e.g., switch between axes after certain threshold)
    static bool rol = true;
    if (m_rotation % 367 == 0) {
        rol = !rol;
    }
    if (rol) {
        m.rotate(m_rotation, 0, 1, 1);  // Custom Z-axis rotation
    }
    else {
        m.rotate(m_rotation, 1, 0, 1);  // Custom X-axis rotation
    }

}

