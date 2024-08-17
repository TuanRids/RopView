#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include "ITcg.h"

namespace Instrumentation
{
  class ITcgType
  {
  public:
    virtual ~ITcgType() = default;

    virtual Decibel GetMinGain() const = 0;
    virtual Decibel GetMaxGain() const = 0;
    virtual NanoSecond GetMinDelayOnMainBang() const = 0;
    virtual NanoSecond GetMinDelayOnGateEvent() const = 0;
    virtual Decibel GetMaxSlopePerNanoSecond() const = 0;
    virtual NanoSecond GetTimeStepMin() const = 0;
    virtual std::size_t GetMaxPointsQty() const = 0;
    virtual bool IsFirstPointGainFixed() const = 0;
    virtual ITcg::Type GetType() const = 0;
  };

  using ITcgTypePtr = std::shared_ptr<ITcgType>;
  using ITcgTypeConstPtr = std::shared_ptr<const ITcgType>;
}
