///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IGateConfigurationFunction.h.
///
/// Declares the IGateConfigurationFunction interface
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace Olympus
{
  namespace Inspection
  {
    /// Values that represent event types
    enum class EventType { Peak, Crossing };

    /// Values that represent amplitude types
    enum class AmplitudeType { FirstPeak, SecondPeak, MaximumPeak, LastPeak };

    class IGateConfigurationFunction
    {
    public:

      virtual ~IGateConfigurationFunction() = default;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets event type
    ///
    /// NDE file domain path sectorialPulseEcho / gates[index] / timeSelection
    /// 
    /// @returns The event type.
    ///////////////////////////////////////////////////////////////////////////////////////////////
      virtual EventType GetEventType() const = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets amplitude type
    ///
    /// NDE file domain path sectorialPulseEcho / gates[index] / peakDetection
    /// 
    /// @returns The amplitude type.
    ///////////////////////////////////////////////////////////////////////////////////////////////
      virtual AmplitudeType GetAmplitudeType() const = 0;

    };

    using IGateConfigurationFunctionPtr = std::shared_ptr<IGateConfigurationFunction>;
    using IGateConfigurationFunctionConstPtr = std::shared_ptr<const IGateConfigurationFunction>;
  }
}

