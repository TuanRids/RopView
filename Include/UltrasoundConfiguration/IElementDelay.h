////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IElementDelay.h
///
/// Declares the IDelay interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IElementDelay
  {
  public:
    virtual ~IElementDelay() = default;

    virtual std::size_t GetElementId() const = 0;
    virtual void SetElementId(std::size_t id) = 0;
    virtual NanoSecond GetDelay() const = 0;
    virtual bool SetDelay(NanoSecond delay) = 0;
  };

  using IElementDelayPtr = std::shared_ptr<IElementDelay>;
  using IElementDelayConstPtr = std::shared_ptr<const IElementDelay>;
}
