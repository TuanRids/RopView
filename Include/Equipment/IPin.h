////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IPin.h.
///
/// NDE file domain path: Not available
/// 
/// Declares the IPin interface.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus
{
  namespace Equipment
  {
    /// Values that represent pin signals
    enum class PinSignal {
      PhaseA_axis1,
      PhaseB_axis1,
      PhaseB_axis2,
      PhaseA_axis2,
      DIN1,
      DIN2,
      DIN3,
      DIN4
    };

    class IPin
    {
    public:
      virtual ~IPin() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the identifier of the pin
      ///
      /// @returns The identifier.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetId() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the signal on a pin
      ///
      /// @returns The signal.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual PinSignal GetSignal() const = 0;
    };

    using IPinPtr = std::shared_ptr<IPin>;
    using IPinConstPtr = std::shared_ptr<const IPin>;
  }
}
