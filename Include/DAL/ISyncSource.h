#pragma once
#include <ultrasoundConfiguration/ITimeSettings.h>

namespace Instrumentation
{
  class ISyncSource
  {
  public:
    virtual ~ISyncSource() = default;

    virtual ITimeSettings::AscanSynchroMode GetMode() const = 0;
    virtual bool IsSaturated() const = 0;
  };

  using ISyncSourcePtr = std::shared_ptr<ISyncSource>;
  using ISyncSourceConstPtr = std::shared_ptr<const ISyncSource>;
}
