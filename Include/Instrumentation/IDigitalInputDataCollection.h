#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IDigitalInputData;

  class IDigitalInputDataCollection
  {
  public:
    virtual ~IDigitalInputDataCollection() = default;

    virtual std::size_t GetCount() const noexcept = 0;
    virtual std::shared_ptr<IDigitalInputData> Get(std::size_t) const = 0;
  };
}
