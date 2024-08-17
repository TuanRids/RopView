#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class ITcgPoint
  {
  public:
    virtual ~ITcgPoint() = default;

    virtual double GetGain() const = 0;
    virtual void SetGain(double gain) = 0;
    virtual NanoSecond GetPosition() const = 0;
    virtual void SetPosition(NanoSecond position) = 0;
  };

  using ITcgPointPtr = std::shared_ptr<ITcgPoint>;
  using ITcgPointConstPtr = std::shared_ptr<const ITcgPoint>;
}
