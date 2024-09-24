#include "MainViewport.h"

//QVulkanWindowRenderer* VulkanWindow::createRenderer() { return new VulkanRenderer(this); };

void VulkanWindow::GetDeviceInfo()
{
    QVulkanInstance* inst = this->vulkanInstance();
    auto m_devFuncs = inst->deviceFunctions(this->device());

    QString info;
    info += "Number of physical devices: " + QString::number(this->availablePhysicalDevices().count()) + "\n";

    QVulkanFunctions* f = inst->functions();
    VkPhysicalDeviceProperties props;
    f->vkGetPhysicalDeviceProperties(this->physicalDevice(), &props);
    info += "Active physical device name: " + QString::fromUtf8(props.deviceName) + "\n";
    info += "API version: " + QString::number(VK_VERSION_MAJOR(props.apiVersion)) + "."
        + QString::number(VK_VERSION_MINOR(props.apiVersion)) + "."
        + QString::number(VK_VERSION_PATCH(props.apiVersion)) + "\n";

    info += "Supported instance layers:\n";
    for (const QVulkanLayer& layer : inst->supportedLayers())
        info += "    " + layer.name + " v" + QString::number(layer.version) + "\n";

    info += "Enabled instance layers:\n";
    for (const QByteArray& layer : inst->layers())
        info += "    " + QString(layer) + "\n";

    info += "Supported instance extensions:\n";
    for (const QVulkanExtension& ext : inst->supportedExtensions())
        info += "    " + ext.name + " v" + QString::number(ext.version) + "\n";

    info += "Enabled instance extensions:\n";
    for (const QByteArray& ext : inst->extensions())
        info += "    " + QString(ext) + "\n";

    info += "Color format: " + QString::number(this->colorFormat()) + "\n";
    info += "Depth-stencil format: " + QString::number(this->depthStencilFormat()) + "\n";

    info += "Supported sample counts: ";
    const QList<int> sampleCounts = this->supportedSampleCounts();
    for (int count : sampleCounts)
        info += QString::number(count) + " ";
    info += "\n";

    nmainUI::statuslogs::getinstance().logInfo(info.toStdString());
}
