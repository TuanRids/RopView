////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IDigitalInputPin.h
///
/// Declares the IDigitalInputPin interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IDigitalInputPin
  {
  public:
    using Action = unsigned;

    static constexpr Action None = 0;
    static constexpr Action ExternalPace = 1 << 0;
    static constexpr Action PresetEncoder1 = 1 << 1;
    static constexpr Action PresetEncoder2 = 1 << 2;
    static constexpr Action EnableFiring = 1 << 3;

    virtual ~IDigitalInputPin() = default;

    virtual int GetPinId() const = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the state of the pin. HIGH or LOW
    ///
    /// @return
    /// The state of the pin.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual DigitalPinState GetState() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @return
    /// The bitmask of the actions of the pin.
    ///
    /// The AND '&' operator needs to be used in order to know if an action other than "None" is set
    /// If "None" is set, the return value will be "None"
    /// Ex :  if(pin->GetPinAction() & ExternalPace) // Check if "ExternalPace" is set
    /// Ex :  if(pin->GetPinAction() == (ExternalPace | EnableFiring) // Check if both "ExternalPace" and "EnableFiring" are the only ones set
    /// Ex :  if(pin->GetPinAction() == None) // Check if "None" is the only action set
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Action GetPinAction() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the actions for the pin.
    /// The actions are set instantly.
    /// The actions are triggered when the state of the pin pass from DigitalPinState::Low to DigitalPinState::High
    ///
    /// @param action bitmask
    /// To set multiple actions on the pin, use the OR '|' operator on the same call
    /// Calling this method multiple time will not append actions on the pin. Only the actions of the last call are effective
    /// If the action "None" is set along with others, it will be ignored
    /// 
    /// Ex : pin->SetPinAction(ExternalPace | EnableFiring); // Set 2 actions
    /// Ex : pin->SetPinAction(None); // Set "None" action
    /// Ex : pin->SetPinAction(ExternalPace | None); // Set "ExternalPace"."None" is ignored
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetPinAction(Action action) = 0;
  };

  using IDigitalInputPinPtr = std::shared_ptr<IDigitalInputPin>;
  using IDigitalInputPinConstPtr = std::shared_ptr<const IDigitalInputPin>;
}
