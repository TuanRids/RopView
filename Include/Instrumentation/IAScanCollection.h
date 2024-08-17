#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IAscan;

  class IAscanCollection
  {
  public:
    virtual ~IAscanCollection() = default;

    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<IAscan> GetAscan(std::size_t index) const = 0;
  };
}
