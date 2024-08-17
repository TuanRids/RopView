#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class ICscan;

  class ICscanCollection
  {
  public:
    virtual ~ICscanCollection() = default;

    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<ICscan> GetCscan(std::size_t index) const = 0;
  };
}
