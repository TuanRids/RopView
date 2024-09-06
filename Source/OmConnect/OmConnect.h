#pragma once
#include "..\pch.h"
#include "readconfig.h"
using namespace Instrumentation;
// vulkan & QT
/*
    # https://cmake.org/cmake/help/latest/module/FindVulkan.html
    find_package(Vulkan REQUIRED)
    target_link_libraries(main PRIVATE Vulkan::Vulkan)

qtbase provides CMake targets:

  # this is heuristically generated, and may not be correct
  find_package(Qt6 CONFIG REQUIRED)
  # note: 45 additional targets are not displayed.
  target_link_libraries(main PRIVATE DB2::DB2 PPS::PPS Oracle::OCI Libb2::Libb2)

  find_package(Qt6Concurrent CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::Concurrent Qt6::Concurrent Qt::ConcurrentPrivate Qt6::ConcurrentPrivate)

  find_package(Qt6Core CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::Core Qt6::Core Qt::CorePrivate Qt6::CorePrivate)

  find_package(Qt6DBus CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::DBus Qt6::DBus Qt::DBusPrivate Qt6::DBusPrivate)

  find_package(Qt6DeviceDiscoverySupportPrivate CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::DeviceDiscoverySupportPrivate Qt6::DeviceDiscoverySupportPrivate)

  find_package(Qt6EntryPointPrivate CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::EntryPointPrivate Qt6::EntryPointPrivate Qt6::EntryPointImplementation)

  find_package(Qt6ExampleIconsPrivate CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::ExampleIconsPrivate Qt6::ExampleIconsPrivate Qt6::ExampleIconsPrivate_resources_1)

  find_package(Qt6FbSupportPrivate CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::FbSupportPrivate Qt6::FbSupportPrivate)

  find_package(Qt6Gui CONFIG REQUIRED)
  # note: 8 additional targets are not displayed.
  target_link_libraries(main PRIVATE Qt::Gui Qt6::Gui Qt::GuiPrivate Qt6::GuiPrivate)

  find_package(Qt6Network CONFIG REQUIRED)
  # note: 4 additional targets are not displayed.
  target_link_libraries(main PRIVATE Qt::Network Qt6::Network Qt6::QNLMNIPlugin Qt::NetworkPrivate)

  find_package(Qt6OpenGL CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::OpenGL Qt6::OpenGL Qt::OpenGLPrivate Qt6::OpenGLPrivate)

  find_package(Qt6OpenGLWidgets CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::OpenGLWidgets Qt6::OpenGLWidgets Qt::OpenGLWidgetsPrivate Qt6::OpenGLWidgetsPrivate)

  find_package(Qt6PrintSupport CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::PrintSupport Qt6::PrintSupport Qt::PrintSupportPrivate Qt6::PrintSupportPrivate)

  find_package(Qt6Sql CONFIG REQUIRED)
  # note: 2 additional targets are not displayed.
  target_link_libraries(main PRIVATE Qt::Sql Qt6::Sql Qt::SqlPrivate Qt6::SqlPrivate)

  find_package(Qt6Test CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::Test Qt6::Test Qt::TestPrivate Qt6::TestPrivate)

  find_package(Qt6Widgets CONFIG REQUIRED)
  # note: 1 additional targets are not displayed.
  target_link_libraries(main PRIVATE Qt::Widgets Qt6::Widgets Qt::WidgetsPrivate Qt6::WidgetsPrivate)

  find_package(Qt6Xml CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::Xml Qt6::Xml Qt::XmlPrivate Qt6::XmlPrivate)

qt3d provides CMake targets:

  # this is heuristically generated, and may not be correct
  find_package(Qt63DAnimation CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::3DAnimation Qt6::3DAnimation Qt::3DAnimationPrivate Qt6::3DAnimationPrivate)

  find_package(Qt63DCore CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::3DCore Qt6::3DCore Qt::3DCorePrivate Qt6::3DCorePrivate)

  find_package(Qt63DExtras CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::3DExtras Qt6::3DExtras Qt::3DExtrasPrivate Qt6::3DExtrasPrivate)

  find_package(Qt63DInput CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::3DInput Qt6::3DInput Qt::3DInputPrivate Qt6::3DInputPrivate)

  find_package(Qt63DLogic CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Qt::3DLogic Qt6::3DLogic Qt::3DLogicPrivate Qt6::3DLogicPrivate)

  find_package(Qt63DRender CONFIG REQUIRED)
  # note: 7 additional targets are not displayed.
  target_link_libraries(main PRIVATE Qt::3DRender Qt6::3DRender Qt::3DRenderPrivate Qt6::3DRenderPrivate)

VulkanMemoryAllocator provides official find_package support. However, it requires the user to provide the include directory containing `vulkan/vulkan.h`. There are multiple ways to achieve this and VulkanMemoryAllocator is compatible with all of them.

    find_package(Vulkan) # https://cmake.org/cmake/help/latest/module/FindVulkan.html, CMake 3.21+
    find_package(VulkanMemoryAllocator CONFIG REQUIRED)
    target_link_libraries(main PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

or

    find_package(Vulkan) # CMake 3.21+
    find_package(VulkanMemoryAllocator CONFIG REQUIRED)
    target_link_libraries(main PRIVATE Vulkan::Headers GPUOpen::VulkanMemoryAllocator)

or

    find_package(VulkanHeaders CONFIG) # From the vulkan-headers port
    find_package(VulkanMemoryAllocator CONFIG REQUIRED)
    target_link_libraries(main PRIVATE Vulkan::Headers GPUOpen::VulkanMemoryAllocator)

See the documentation for more information on setting up your project: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html

The package vulkan-validationlayers provides the vulkan validationlayers.
To make vulkan use them you have to manually add the vcpkg path "<vcpkg_installed>/bin" to the environment variable VK_ADD_LAYER_PATH.


C:\Users\ngdtu>*/

using namespace std;
namespace omcn
{
    class OmConnect
    {
    private:
        // properties
        shared_ptr<IDevice> device;
        string ipAddress = "172.17.0.1";

        // private methods
        shared_ptr<IDevice> DiscoverDevice();
        void StartDevice();
        void ConfigureDevice();
    public:
        OmConnect(): device(nullptr) {};
        ~OmConnect() {};
        bool omConnectDevice();
    };

}
