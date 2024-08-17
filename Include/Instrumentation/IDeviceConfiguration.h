#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include <instrumentation/IGpioConnectorCollection.h>
#include <instrumentation/IDeviceInfo.h>

#if defined(INSTRUMENTATION_EXPORTS)
#define INSTRUMENTATION_API __declspec(dllexport)
#else
#define INSTRUMENTATION_API __declspec(dllimport)
#endif

namespace Instrumentation
{
  class IUltrasoundConfiguration;

  class IDeviceConfiguration
  {
  public:
    virtual ~IDeviceConfiguration() = default;

    virtual std::shared_ptr<IUltrasoundConfiguration> GetUltrasoundConfiguration() const = 0;

    virtual std::shared_ptr<IGpioConnectorCollection> GetGpioConnectorCollection() const = 0;

    __declspec(deprecated("Deprecated."))
    static std::shared_ptr<IDeviceConfiguration> INSTRUMENTATION_API Create(IDeviceInfo::Platform platform, IDeviceInfo::Model model);

    virtual const std::wstring& GetDeviceName() const = 0;

    virtual void SetDeviceName(const std::wstring& name) = 0;

    virtual IDeviceInfo::Platform GetPlatform() const = 0;

    virtual IDeviceInfo::Model GetModel() const = 0;

    virtual const std::string& GetSerialNumber() const = 0;
  };

  using IDeviceConfigurationPtr = std::shared_ptr<IDeviceConfiguration>;
  using IDeviceConfigurationConstPtr = std::shared_ptr<const IDeviceConfiguration>;
}
