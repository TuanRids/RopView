#include "TriangleTModel.h"


static uint16_t indices[] = {
    0, 1, 2,0,2,3 // First triangle
};

static const int UNIFORM_DATA_SIZE = 16 * sizeof(float);

static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

TriangleRenderer::TriangleRenderer(QVulkanWindow* w, bool msaa)
    : m_VulWindow(w)
{
    omesh.push_back(Mesh());    
    omesh.push_back(Mesh());

    const int gridSize = 3;
    for (int i = 0; i <= gridSize; ++i) {
        for (int j = 0; j <= gridSize; ++j) {
            float x = (float(i) / gridSize) - 0.2f;
            float y = (float(j) / gridSize) - 0.2f;
            omesh[0].vertices.push_back(Vertex(glm::vec3(x, y, 0.0f), glm::vec3(0.3f+i*0.1f, 0.4f+j*0.1f, 0.5f)));
        }
    }

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            uint16_t topLeft = i * (gridSize + 1) + j;
            uint16_t topRight = topLeft + 1;
            uint16_t bottomLeft = (i + 1) * (gridSize + 1) + j;
            uint16_t bottomRight = bottomLeft + 1;

            omesh[0].indices.push_back(topLeft);
            omesh[0].indices.push_back(bottomLeft);
            omesh[0].indices.push_back(topRight);

            omesh[0].indices.push_back(topRight);
            omesh[0].indices.push_back(bottomLeft);
            omesh[0].indices.push_back(bottomRight);
        }
    }

    omesh[1].vertices = {
        // Front face
        Vertex(glm::vec3(-0.2f,  0.2f,  0.2f), glm::vec3(0.2f, 0.6f, 0.3f)), // red
        Vertex(glm::vec3(0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 0.3f, 1.0f)), // green
        Vertex(glm::vec3(0.2f, -0.2f,  0.2f), glm::vec3(0.6f, 0.0f, 0.2f)), // blue
        Vertex(glm::vec3(-0.2f, -0.2f,  0.2f), glm::vec3(0.5f, 0.6f, 0.0f)), // yellow

        // Back face
        Vertex(glm::vec3(-0.2f, 0.2f, -0.2f), glm::vec3(0.4f, 0.2f, 0.2f)), // cyan
        Vertex(glm::vec3(0.2f,  0.2f, -0.2f), glm::vec3(0.1f, 0.7f, 0.8f)), // magenta
        Vertex(glm::vec3(0.2f, -0.2f, -0.2f), glm::vec3(0.6f, 0.1f, 0.4f)), // orange
        Vertex(glm::vec3(-0.2f, -0.2f, -0.2f), glm::vec3(0.2f, 0.4f, 0.6f))  // gray
    };
    omesh[1].indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1,
        4, 5, 1, 1, 0, 4,
        3, 2, 6, 6, 7, 3
    };

    bufin = std::make_shared<BufIN>();
    desin = std::make_shared<DesIN>();
    pipein = std::make_shared<PipeIN>();
    if (!pSttLogs) { pSttLogs = &nmainUI::statuslogs::getinstance(); }
    if (msaa) {
        const QList<int> counts = w->supportedSampleCounts();
        for (int s = 16; s >= 4; s /= 2) { if (counts.contains(s)) { m_VulWindow->setSampleCount(s); break; } }
    }
}



void TriangleRenderer::initResources()
{
    m_deviFunc = m_VulWindow->vulkanInstance()->deviceFunctions(m_VulWindow->device());
        
    std::unique_ptr<ResourceBuilder> upResBuilder = std::make_unique<ResourceBuilder>(m_deviFunc, m_VulWindow->device(), bufin,desin, pipein);
    try {
        for (auto& mesh : omesh) {
            upResBuilder->createBuffer(m_VulWindow, mesh);
            upResBuilder->allocateMemory(m_VulWindow, mesh);
            mesh.buffer = bufin->buffer;
            mesh.idbuffer = bufin->idbuffer;
        }
        upResBuilder->createDescriptor();
        upResBuilder->createPipeLine(m_VulWindow);
    }
    catch (std::exception& e) {
        qFatal("%s", e.what());
    }
}

void TriangleRenderer::initSwapChainResources()
{
    qDebug("initSwapChainResources");

    // Projection matrix
    m_proj = m_VulWindow->clipCorrectionMatrix(); // adjust for Vulkan-OpenGL clip space differences
    const QSize sz = m_VulWindow->swapChainImageSize();
    m_proj.perspective(45.0f, sz.width() / (float)sz.height(), 0.01f, 100.0f);
    m_proj.translate(0, 0, -4);
}

void TriangleRenderer::releaseSwapChainResources()
{
    qDebug("releaseSwapChainResources");
}

void TriangleRenderer::releaseResources()
{
    qDebug("releaseResources");

    VkDevice dev = m_VulWindow->device();

    if (pipein->pipeline) {
        m_deviFunc->vkDestroyPipeline(dev, pipein->pipeline, nullptr);
        pipein->pipeline = VK_NULL_HANDLE;
    }

    if (pipein->pipelineLayout) {
        m_deviFunc->vkDestroyPipelineLayout(dev, pipein->pipelineLayout, nullptr);
        pipein->pipelineLayout = VK_NULL_HANDLE;
    }

    if (pipein->pipelineCache) {
        m_deviFunc->vkDestroyPipelineCache(dev, pipein->pipelineCache, nullptr);
        pipein->pipelineCache = VK_NULL_HANDLE;
    }

    if (desin->descSetLayout) {
        m_deviFunc->vkDestroyDescriptorSetLayout(dev, desin->descSetLayout, nullptr);
        desin->descSetLayout = VK_NULL_HANDLE;
    }

    if (desin->descPool) {
        m_deviFunc->vkDestroyDescriptorPool(dev, desin->descPool, nullptr);
        desin->descPool = VK_NULL_HANDLE;
    }

    if (bufin->buffer) {
        m_deviFunc->vkDestroyBuffer(dev, bufin->buffer, nullptr);
        bufin->buffer = VK_NULL_HANDLE;
    }
    if (bufin->idbuffer) {
        m_deviFunc->vkDestroyBuffer(dev, bufin->idbuffer, nullptr);
        bufin->idbuffer = VK_NULL_HANDLE;
    }

    if (bufin->memory) {
        m_deviFunc->vkFreeMemory(dev, bufin->memory, nullptr);
        bufin->memory = VK_NULL_HANDLE;
    }
    if (bufin->indexmemory) {
        m_deviFunc->vkFreeMemory(dev, bufin->indexmemory, nullptr);
        bufin->indexmemory = VK_NULL_HANDLE;
    }
}

void TriangleRenderer::startNextFrame() {
    VkDevice dev = m_VulWindow->device();
    VkCommandBuffer cb = m_VulWindow->currentCommandBuffer();
    const QSize sz = m_VulWindow->swapChainImageSize();

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

    VkCommandBuffer cmdBuf = m_VulWindow->currentCommandBuffer();
    m_deviFunc->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Update projection matrix and rotation
    quint8* p;
    VkResult err = m_deviFunc->vkMapMemory(dev, bufin->memory, bufin->uniformBufferInfo[m_VulWindow->currentFrame()].offset,
        UNIFORM_DATA_SIZE, 0, reinterpret_cast<void**>(&p));
    if (err != VK_SUCCESS) {
        qFatal("Failed to map memory: %d", err);
    }

    QMatrix4x4 m = m_proj;
    m.rotate(m_rotationX, 0, 1, 0);  // Rotate on Y-axis
    m.rotate(m_rotationY, 1, 0, 0);  // Rotate on X-axis
    static int direction = 0;
    static bool rol = true;
    if (m_rotation % 367 == 0) {
        rol = !rol;
    }
    if (rol){
		m.rotate(m_rotation, 0, 1, 1);
	}
	else {
		m.rotate(m_rotation, 1, 0, 1);
	}
    m_rotation++;
    
    memcpy(p, m.constData(), 16 * sizeof(float));
    m_deviFunc->vkUnmapMemory(dev, bufin->memory);

    // Iterate through omesh to render each one
    for (const auto& mesh : omesh) {
        // Bind the pipeline and descriptor sets
        m_deviFunc->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipein->pipeline);
        m_deviFunc->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipein->pipelineLayout, 0, 1,
            &desin->descSet[m_VulWindow->currentFrame()], 0, nullptr);

        // Bind vertex and index buffers
        VkDeviceSize vbOffset = 0;
        m_deviFunc->vkCmdBindVertexBuffers(cb, 0, 1, &mesh.buffer, &vbOffset);
        m_deviFunc->vkCmdBindIndexBuffer(cb, mesh.idbuffer, 0, VK_INDEX_TYPE_UINT16);

        // Draw indexed vertices
        m_deviFunc->vkCmdDrawIndexed(cb, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
    }

    // Set the viewport and scissor
    VkViewport viewport;
    viewport.x = viewport.y = 0;
    viewport.width = sz.width();
    viewport.height = sz.height();
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    m_deviFunc->vkCmdSetViewport(cb, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewport.width;
    scissor.extent.height = viewport.height;
    m_deviFunc->vkCmdSetScissor(cb, 0, 1, &scissor);

    m_deviFunc->vkCmdEndRenderPass(cmdBuf);

    m_VulWindow->frameReady();
    m_VulWindow->requestUpdate();
}



void TriangleRenderer::updateRotation(float deltaX, float deltaY)
{
    m_rotationX += deltaX * 0.5f;  // Sensitivity factor
    m_rotationY += deltaY * 0.5f;
}

