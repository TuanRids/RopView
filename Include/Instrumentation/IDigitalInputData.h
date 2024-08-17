#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IDigitalInputData
  {
  public:
    virtual ~IDigitalInputData() = default;

    virtual std::string GetDeviceSerialNumber() const = 0;
    virtual int GetPinId() const noexcept = 0;
    virtual DigitalPinState GetState() const noexcept = 0;
    virtual const std::wstring& GetConnectorName() const noexcept = 0;
  };
}
