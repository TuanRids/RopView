#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IEncoderData;

  class IEncoderDataCollection
  {
  public:
    virtual ~IEncoderDataCollection() = default;
    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<IEncoderData> GetEncoderData(std::size_t index) const = 0;
  };
}
