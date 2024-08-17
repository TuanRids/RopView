#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IEncoder
  {
  public:
    enum class Type { ClockDir, Quadrature, PulseUp, PulseDown };

    virtual ~IEncoder() = default;

    virtual void Enable(bool doEnable) = 0;
    virtual bool IsEnabled() const = 0;

    virtual Type GetEncoderType() const = 0;
    virtual void SetEncoderType(Type type) = 0;

    virtual std::size_t GetResolution() const = 0;
    virtual void SetResolution(std::size_t resolution) = 0;

    virtual std::size_t GetPresetValue() const = 0;
    virtual void SetPresetValue(std::size_t presetValue) = 0;

    virtual const std::wstring& GetName() const = 0;
    virtual void SetName(const std::wstring& name) = 0;
  };
}
