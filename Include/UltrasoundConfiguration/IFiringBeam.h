#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IBeam;

  class IFiringBeam
  {
  public:
    virtual ~IFiringBeam() = default;

    virtual std::shared_ptr<IBeam> GetBeam() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the position of the beam in the firing order of the whole device. (e.g. 1st, 6th, etc.).
    ///
    /// @return
    /// The firing order.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetFiringOrder() const = 0;

    virtual void SetFiringOrder(std::size_t firingOrder) = 0;
  };

  using IFiringBeamPtr = std::shared_ptr<IFiringBeam>;
  using IFiringBeamConstPtr = std::shared_ptr<const IFiringBeam>;
}
