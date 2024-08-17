#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include <instrumentation/IEncoder.h>

namespace Instrumentation
{
  class IEncoderCollection
  {
  public:
    virtual ~IEncoderCollection() = default;
    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<IEncoder> GetEncoder(std::size_t index) const = 0;
  };
}
