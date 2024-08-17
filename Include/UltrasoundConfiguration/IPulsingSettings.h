////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IPulsingSettings.h
///
/// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
/// or compoundPulseEcho
/// 
/// Declares the IPulsingSettings interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IPulsingSettings
  {
  public:

    virtual ~IPulsingSettings() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the pulse width which represents the time duration of each beam's pulse for this beam set.
    /// The pulse width is the same for all elements of the beam. The pulse width and the pulsing 
    /// voltage of the connector can limit the acquisition rate.
    ///
    /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
    /// or compoundPulseEcho / pulseWidth
    /// 
    /// @return
    /// The pulse width in NanoSecond.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual NanoSecond GetPulseWidth() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the pulse width which represents the time duration of each beam's pulse for this beam set.
    /// The pulse width is the same for all elements of the beam. The pulse width and the pulsing 
    /// voltage of the connector can limit the acquisition rate.
    ///
    /// @param width The desired pulse width in NanoSeconds.
    ///
    /// @return
    /// A boolean returning success status:
    /// True if the pulse width value applied was the given value.
    /// False if the pulse width set is not equal to the given pulse width.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool SetPulseWidth(NanoSecond width) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the averaging factor. 
    /// 
    /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
    /// or compoundPulseEcho / averagingFactor
    /// 
    /// @return
    /// The averaging factor.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class AveragingFactor
    {
      One = 1,
      Two = 2,
      Four = 4,
      Eight = 8,
      Sixteen = 16,
      ThirtyTwo = 32,
      SixtyFour = 64
    };

    virtual AveragingFactor GetAscanAveragingFactor() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the averaging factor.
    /// 
    /// @param factor The desired averaging factor.
    /// 
    /// @return
    /// A boolean returning success status:
    /// True if the requested value was applied,
    /// False if adjusted to a lower supported value.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool SetAscanAveragingFactor(AveragingFactor factor) = 0;

    virtual void Enable(bool doEnable) = 0;

    virtual bool IsEnabled() const = 0;
  };

  using IPulsingSettingsPtr = std::shared_ptr<IPulsingSettings>;
  using IPulsingSettingsConstPtr = std::shared_ptr<const IPulsingSettings>;
}
