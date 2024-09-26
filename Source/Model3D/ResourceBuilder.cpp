#include "ResourceBuilder.h"
static const int UNIFORM_DATA_SIZE = 16 * sizeof(float);
int ResourceBuilder::concurrentFrameCount = 0;

static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

ResourceBuilder& ResourceBuilder::createBuffer(QVulkanWindow* m_VulWindow, Mesh& gmesh)
{
    concurrentFrameCount = m_VulWindow->concurrentFrameCount();
    VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };

    const VkDeviceSize uniAlign = m_VulWindow->physicalDeviceProperties()->limits.minUniformBufferOffsetAlignment;
    bufin->vertexAllocSize = aligned(gmesh.vertices.size() * sizeof(Vertex), uniAlign);
    bufin->indexAllocSize = aligned(gmesh.indices.size() * sizeof(uint16_t), uniAlign);
    bufin->uniformAllocSize = aligned(UNIFORM_DATA_SIZE, uniAlign);

    bufInfo.size = bufin->vertexAllocSize + bufin->indexAllocSize + concurrentFrameCount * bufin->uniformAllocSize;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    VkResult result = m_deviFunc->vkCreateBuffer(m_device, &bufInfo, nullptr, &bufin->buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer.");
    }

    VkBufferCreateInfo indexBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    indexBufferInfo.size = bufin->indexAllocSize;
    indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    result = m_deviFunc->vkCreateBuffer(m_device, &indexBufferInfo, nullptr, &bufin->idbuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create index buffer.");
    }

    return *this;
}


ResourceBuilder& ResourceBuilder::allocateMemory(QVulkanWindow* m_VulWindow, Mesh& gmesh)
{
    // Alloc and bind vertex and index buffer memory
    VkMemoryRequirements memReq = {};
    m_deviFunc->vkGetBufferMemoryRequirements(m_device, bufin->buffer, &memReq);
    VkMemoryAllocateInfo memAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, memReq.size, m_VulWindow->hostVisibleMemoryIndex() };

    VkResult result = m_deviFunc->vkAllocateMemory(m_device, &memAllocInfo, nullptr, &bufin->memory);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate memory."); }

    result = m_deviFunc->vkBindBufferMemory(m_device, bufin->buffer, bufin->memory, 0);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to bind vertex buffer memory."); }

    // Allocate index buffer memory
    m_deviFunc->vkGetBufferMemoryRequirements(m_device, bufin->idbuffer, &memReq);
    memAllocInfo.allocationSize = memReq.size;
    result = m_deviFunc->vkAllocateMemory(m_device, &memAllocInfo, nullptr, &bufin->indexmemory);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate index buffer memory."); }

    result = m_deviFunc->vkBindBufferMemory(m_device, bufin->idbuffer, bufin->indexmemory, bufin->indexAllocSize);  // Bind after vertex buffer
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to bind index buffer memory."); }

    // Map and copy vertex data
    quint8* p = nullptr;
    result = m_deviFunc->vkMapMemory(m_device, bufin->memory, 0, memReq.size, 0, reinterpret_cast<void**>(&p));
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to map vertex memory."); }
    memcpy(p, gmesh.vertices.data(), gmesh.vertices.size() * sizeof(Vertex));
    m_deviFunc->vkUnmapMemory(m_device, bufin->memory);

    // Map and copy index data (right after vertex data)
    result = m_deviFunc->vkMapMemory(m_device, bufin->indexmemory, bufin->indexAllocSize, gmesh.indices.size(), 0, reinterpret_cast<void**>(&p));
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to map index memory."); }
    memcpy(p, gmesh.indices.data(), gmesh.indices.size()*sizeof(uint16_t));
    m_deviFunc->vkUnmapMemory(m_device, bufin->indexmemory);

    // Set up uniform buffer info based on vertex data
    QMatrix4x4 ident;
    memset(bufin->uniformBufferInfo, 0, sizeof(bufin->uniformBufferInfo));
    for (int i = 0; i < concurrentFrameCount; ++i) {
        const VkDeviceSize offset = bufin->vertexAllocSize + i * bufin->uniformAllocSize;
        memcpy(p + offset, ident.constData(), 16 * sizeof(float));
        bufin->uniformBufferInfo[i].buffer = bufin->buffer;
        bufin->uniformBufferInfo[i].offset = offset;
        bufin->uniformBufferInfo[i].range = bufin->uniformAllocSize;
    }

    return *this;
}

ResourceBuilder& ResourceBuilder::createDescriptor( )
{

    // Set up and create descriptor pool
    VkDescriptorPoolSize descPoolSizes = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uint32_t(concurrentFrameCount) };
    VkDescriptorPoolCreateInfo descPoolInfo;
    memset(&descPoolInfo, 0, sizeof(descPoolInfo));
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.maxSets = concurrentFrameCount;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSizes;
    result = m_deviFunc->vkCreateDescriptorPool(m_device, &descPoolInfo, nullptr, &desin->descPool);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create descriptor pool."); }

    // Set up and create descriptor layout
    VkDescriptorSetLayoutBinding layoutBinding = { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr };
    VkDescriptorSetLayoutCreateInfo descLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,nullptr,0,1,&layoutBinding };
    result = m_deviFunc->vkCreateDescriptorSetLayout(m_device, &descLayoutInfo, nullptr, &desin->descSetLayout);
	if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create descriptor set layout."); }

    // Set up and create descriptor set
    for (int i = 0; i < concurrentFrameCount; ++i) {
        VkDescriptorSetAllocateInfo descSetAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, desin->descPool, 1, &desin->descSetLayout };
        result = m_deviFunc->vkAllocateDescriptorSets(m_device, &descSetAllocInfo, &desin->descSet[i]);
		if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate descriptor set."); }
        VkWriteDescriptorSet descWrite;
        memset(&descWrite, 0, sizeof(descWrite));
        descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.dstSet = desin->descSet[i];
        descWrite.descriptorCount = 1;
        descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrite.pBufferInfo = &bufin->uniformBufferInfo[i];
        m_deviFunc->vkUpdateDescriptorSets(m_device, 1, &descWrite, 0, nullptr);
    }
}

ResourceBuilder& ResourceBuilder::createPipeLine(QVulkanWindow* m_VulWindow)
{
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    result = m_deviFunc->vkCreatePipelineCache(m_device, &pipelineCacheInfo, nullptr, &pipein->pipelineCache);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create pipeline cache."); }
    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &desin->descSetLayout;
    result = m_deviFunc->vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipein->pipelineLayout);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create pipeline layout."); }
    // Shaders
    VkShaderModule vertShaderModule = createShader(QStringLiteral(":/color_vert.spv"));
    VkShaderModule fragShaderModule = createShader(QStringLiteral(":/color_frag.spv"));

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo shaderStages[2] = { { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule, "main", nullptr },
                { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule, "main", nullptr } };
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;


    VkVertexInputBindingDescription vertexBindingDesc = { 0,sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
    VkVertexInputAttributeDescription vertexAttrDesc[] = {
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },   // Position
        { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) } // ,     // Normal
        //{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) },      // Texture coordinates
        //{ 3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) }       // Color
    };

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;


    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo ia;
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ia.primitiveRestartEnable = VK_FALSE;  // Disable primitive restart for now
    pipelineInfo.pInputAssemblyState = &ia;

    VkPipelineViewportStateCreateInfo vp;
    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    pipelineInfo.pViewportState = &vp;

    VkPipelineRasterizationStateCreateInfo rs;
    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_NONE; // we want the back face as well
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.lineWidth = 1.0f;
    pipelineInfo.pRasterizationState = &rs;

    VkPipelineMultisampleStateCreateInfo ms;
    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Enable multisampling.
    ms.rasterizationSamples = m_VulWindow->sampleCountFlagBits();
    pipelineInfo.pMultisampleState = &ms;

    VkPipelineDepthStencilStateCreateInfo ds;
    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineInfo.pDepthStencilState = &ds;

    VkPipelineColorBlendStateCreateInfo cb;
    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // no blend, write out all of rgba
    VkPipelineColorBlendAttachmentState att;
    memset(&att, 0, sizeof(att));
    att.colorWriteMask = 0xF;
    cb.attachmentCount = 1;
    cb.pAttachments = &att;
    pipelineInfo.pColorBlendState = &cb;

    VkDynamicState dynEnable[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dyn;
    memset(&dyn, 0, sizeof(dyn));
    dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = sizeof(dynEnable) / sizeof(VkDynamicState);
    dyn.pDynamicStates = dynEnable;
    pipelineInfo.pDynamicState = &dyn;

    pipelineInfo.layout = pipein->pipelineLayout;
    pipelineInfo.renderPass = m_VulWindow->defaultRenderPass();

    result = m_deviFunc->vkCreateGraphicsPipelines(m_device, pipein->pipelineCache, 1, &pipelineInfo, nullptr, &pipein->pipeline);
    if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create graphics pipeline."); }
    if (vertShaderModule)
        m_deviFunc->vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
    if (fragShaderModule)
        m_deviFunc->vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
}

VkShaderModule ResourceBuilder::createShader(const QString& name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to read shader %s", qPrintable(name));
        return VK_NULL_HANDLE;
    }
    QByteArray blob = file.readAll();
    file.close();

    VkShaderModuleCreateInfo shaderInfo;
    memset(&shaderInfo, 0, sizeof(shaderInfo));
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(blob.constData());
    VkShaderModule shaderModule;
    VkResult err = m_deviFunc->vkCreateShaderModule(m_device, &shaderInfo, nullptr, &shaderModule);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create shader module: %d", err);
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

