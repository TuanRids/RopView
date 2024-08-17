#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IGate;

  class IGateCollection
  {
  public:
    virtual ~IGateCollection() = default;

    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<IGate> GetGate(std::size_t index) const = 0;
    virtual std::shared_ptr<IGate> FindByName(const std::wstring& name) const = 0;
  };

  using IGateCollectionPtr = std::shared_ptr<IGateCollection>;
  using IGateCollectionConstPtr = std::shared_ptr<const IGateCollection>;
}
