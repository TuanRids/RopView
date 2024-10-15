#include "MeshManager.h"
#include <random>

float randomFloat(float min = 0.01f, float max = 5.08f) {
    std::random_device rd;  // Random seed
    std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_real_distribution<float> dis(min, max); // Uniform distribution between min and max
    return dis(gen);
}

//static uint32_t indices[] = {
//    0, 1, 2,0,2,3 // First triangle
//};

static constexpr int UNIFORM_DATA_SIZE = 16 * sizeof(float);
void MeshRenderer::test_object() {
    const int gridSize = 5;
    // test multiple objects
    unsigned int k = 0;
    for (k = 0; k < 25; ++k)
    {
        omesh.push_back(nullptr);
        omesh[k] = std::make_unique<Mesh>();
        for (int i = 0; i <= gridSize; ++i) {
            for (int j = 0; j <= gridSize; ++j) {
                float x = (float(i) / gridSize) - 0.2f;
                float y = (float(j) / gridSize) - 0.2f;
                float z = 0.1f * cosf(x * 3.1415f) * sinf(y * 3.1415f) + k * 0.25f;
                if (k % 2 == 0) { z = -z; y = -y; x = -x; }
                omesh[k]->vertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0.3f + i * 0.1f * k * 0.2f, 0.4f + j * 0.1f * k * 0.2f, 0.5f)));
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
}
 
MeshRenderer::MeshRenderer(QVulkanWindow* w, bool msaa) :m_VulWindow(w), m_deviFunc(nullptr)
{
    test_object();
    InitAxisGizmo(); 
    if (msaa) { InitMSAA(); } 
    transform = std::make_unique<TransForm>();
}

void MeshRenderer::initResources()
{
    if (!m_deviFunc) 
    { m_deviFunc = m_VulWindow->vulkanInstance()->deviceFunctions(m_VulWindow->device()); }
    if (!upResBuilder) { upResBuilder = std::make_unique<ResourceBuilder>(m_VulWindow); }
    for (auto& mesh : omesh) 
    { upResBuilder->createResources(*mesh); }
}
void MeshRenderer::initSwapChainResources()
{
    //releaseSwapChainResources();
    // Projection matrix
    m_proj = m_VulWindow->clipCorrectionMatrix(); // adjust for Vulkan-OpenGL clip space differences
    auto swapChainSize = m_VulWindow->swapChainImageSize();
    m_proj.perspective(45.0f, swapChainSize.width() / (float)swapChainSize.height(), 0.01f, 100.0f);
    m_proj.translate(0, 0, -4);
}
void MeshRenderer::setupRenderEnvironment(VkCommandBuffer cb, const QSize& sz)
{
    VkClearColorValue clearColor = { { 0.15f, 0.15f, 0.25f, 1 } }; // NOTENOTE COLOR BACKGROUND
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
void MeshRenderer::InitMSAA()
{
    const QList<int> counts = m_VulWindow->supportedSampleCounts();
    for (int s = 16; s >= 4; s /= 2) {
        if (counts.contains(s))
        { m_VulWindow->setSampleCount(s); break; }
    }
}
void MeshRenderer::MeshInit(Mesh& mesh)
{
    upResBuilder->createResources(mesh);
    omesh.push_back(std::make_unique<Mesh>(mesh));
}
void MeshRenderer::InitAxisGizmo()
{
    Mesh newmesh;
    newmesh.systemMesh = true;
    newmesh.name = "axisGizmo";

    newmesh.vertices.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    newmesh.vertices.push_back(Vertex(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    newmesh.vertices.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    newmesh.vertices.push_back(Vertex(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    newmesh.vertices.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    newmesh.vertices.push_back(Vertex(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    newmesh.indices = { 0, 1, 2, 3, 4, 5 };
    omesh.push_back(std::make_unique<Mesh>(newmesh));
}
void MeshRenderer::MeshRelease()
{
}
void MeshRenderer::releaseResources()
{
    m_deviFunc->vkDeviceWaitIdle(m_VulWindow->device());
    for (auto& mesh : omesh) {
        upResBuilder->releaseResources(*mesh);
    }    
}




void MeshRenderer::startNextFrame() {
    VkDevice dev = m_VulWindow->device();
    VkCommandBuffer cb = m_VulWindow->currentCommandBuffer();
    const QSize sz = m_VulWindow->swapChainImageSize();
    VkViewport viewport;
    static float rot_angl = 2.0f, rot_angl2 = 2.0f;

    {
        viewport.x = viewport.y = 0;
        viewport.width = sz.width();
        viewport.height = sz.height();
        viewport.minDepth = 0;
        viewport.maxDepth = 1;

        setupRenderEnvironment(cb, sz);

    }

    for (auto &mesh: omesh) {
        QMatrix4x4 projectionMatrix;
        projectionMatrix.setToIdentity();
        projectionMatrix.perspective(45.0f, float(viewport.width) / viewport.height, 0.1f, 100.0f);
        quint8* p; 
        std::shared_ptr<QMatrix4x4> viewMatrix = std::make_shared<QMatrix4x4>();

        VkResult err = m_deviFunc->vkMapMemory(dev, mesh->memory, mesh->uniformBufferInfo[m_VulWindow->currentFrame()].offset, UNIFORM_DATA_SIZE, 0, reinterpret_cast<void**>(&p));
        if (err != VK_SUCCESS) { qFatal("Failed to map memory: %d", err); }

        viewMatrix->setToIdentity();
        transform->gcam.czoom(transform->wheel_zoom_factor);
        //viewMatrix->translate(0, 0, -4.0f * transform->wheel_zoom_factor); 

        QMatrix4x4 modelMatrix = m_proj;
        if (mesh->name == "axisGizmo") {
            modelMatrix.setToIdentity();
            modelMatrix.perspective(45.0f, float(viewport.width) / viewport.height, 0.1f, 100.0f);
            modelMatrix.translate(-3.0f* float(viewport.width)/1112, 2.1f, -6.0f);
            transform->rotate(modelMatrix);
            transform->scale(modelMatrix, 0.1f);  
            memcpy(p, modelMatrix.constData(), 16 * sizeof(float));
        }
        else {
            modelMatrix.setToIdentity();

            transform->rotate(modelMatrix);
            transform->translate_pan(&*viewMatrix);

            transform->viz_rotate(modelMatrix, rot_angl);
            transform->rotate_cam(&*viewMatrix);
            QMatrix4x4 mvpMatrix = projectionMatrix * *viewMatrix * modelMatrix;
            memcpy(p, mvpMatrix.constData(), 16 * sizeof(float));
        }        
        m_deviFunc->vkUnmapMemory(dev, mesh->memory);
    }

    {
        rot_angl += 2;
        rot_angl2 -= 2;

        m_deviFunc->vkCmdSetViewport(cb, 0, 1, &viewport);
        VkRect2D scissor;
        scissor.offset.x = scissor.offset.y = 0;
        scissor.extent.width = viewport.width;
        scissor.extent.height = viewport.height;
        m_deviFunc->vkCmdSetScissor(cb, 0, 1, &scissor);


        renderMesh(cb);
        m_deviFunc->vkCmdEndRenderPass(cb);
        m_VulWindow->frameReady();
        m_VulWindow->requestUpdate();
    }
}

void MeshRenderer::renderMesh(VkCommandBuffer cb)
{
    for (const auto& mesh : omesh) {
        if (!mesh) { break; }
         //if (mesh->name.find("TestOb_") != std::string::npos)        { continue; }
        // Bind the pipeline and descriptor sets
        m_deviFunc->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh->pipeline);
        m_deviFunc->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh->pipelineLayout, 0, 1,
            &mesh->descSet[m_VulWindow->currentFrame()], 0, nullptr);

        // Bind vertex and index buffers
        VkDeviceSize vbOffset = 0;
        m_deviFunc->vkCmdBindVertexBuffers(cb, 0, 1, &mesh->buffer, &vbOffset);

        VkDeviceSize indexBufferOffset = upResBuilder->aligned(mesh->vertexAllocSize, m_VulWindow->physicalDeviceProperties()->limits.minUniformBufferOffsetAlignment);
        m_deviFunc->vkCmdBindIndexBuffer(cb, mesh->buffer, indexBufferOffset, VK_INDEX_TYPE_UINT32);

        if (mesh->name.find("Paut Object") != std::string::npos) { m_deviFunc->vkCmdDraw(cb, static_cast<uint32_t>(mesh->vertices.size()), 1, 0, 0); continue; }
        // Draw indexed vertices
        m_deviFunc->vkCmdDrawIndexed(cb, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
    }
}

