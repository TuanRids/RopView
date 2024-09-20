////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IDigitalOutputPin.h
///
/// Declares the IDigitalOutputPin interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IDigitalOutputPin
  {
  public:
    virtual ~IDigitalOutputPin() = default;

    virtual int GetPinId() const = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the pin state.
    ///
    /// @return
    /// The state of the pin.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual DigitalPinState GetState() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the state of the pin.
    /// The state is applied instantly.
    ///
    /// @param state
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetState(DigitalPinState state) = 0;
  };

  using IDigitalOutputPinPtr = std::shared_ptr<IDigitalOutputPin>;
  using IDigitalOutputPinConstPtr = std::shared_ptr<const IDigitalOutputPin>;
}