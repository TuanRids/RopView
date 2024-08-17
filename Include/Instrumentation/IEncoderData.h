#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IEncoderData
  {
  public:
    virtual ~IEncoderData() = default;

    virtual std::string GetDeviceSerialNumber() const = 0;
    virtual std::size_t GetCounter() const = 0;
    virtual long GetValue() const = 0;
  };
}
