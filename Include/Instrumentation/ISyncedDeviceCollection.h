#pragma once
#include <instrumentation/InstrumentationExport.h>
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IDevice;

  class INSTRUMENTATION_API ISyncedDeviceCollection
  {
  public:
    virtual ~ISyncedDeviceCollection() = default;

    virtual std::shared_ptr<IDevice> GetDevice(std::size_t index) const = 0;
    virtual std::size_t GetCount() const = 0;
    virtual void Add(std::shared_ptr<IDevice> device) = 0;
    virtual void Remove(std::size_t index) = 0;

    static std::shared_ptr<ISyncedDeviceCollection> Create(std::shared_ptr<IDevice> masterDevice);
  };
}
