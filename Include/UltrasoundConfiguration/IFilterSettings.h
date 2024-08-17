////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IFilterSettings.h
///
/// NDE file domain path:  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
/// or compoundPulseEcho
/// 
/// Declares the IFilterSettings interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IDigitalBandPassFilter;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Controls the settings for band pass and smoothing filters.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IFilterSettings
  {
  public:
    virtual ~IFilterSettings() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the interface for the digital pass filter linked to this beam set. The interface contains
    /// the type of filter (LowPass, HighPass, BandPass or none), the high and low cut off frequency 
    /// as well as the characteristic (TOFD or none) of the filter. 
    ///
    /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
    /// or compoundPulseEcho / digitalBandPassFilter
    /// 
    /// @return
    /// The interface IDigitalBandPassFilter for the filter.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDigitalBandPassFilter> GetDigitalBandPassFilter() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the digital pass filter for this beam set by giving the corresponding 
    /// IDigitalBandPassFilter interface. The collection of supported digital pass
    /// filters can be accessed with DigitizerTechnology::GetDigitalBandPassFilterSupported.
    ///
    /// @return
    /// The interface IDigitalBandPassFilter for the filter.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetDigitalBandPassFilter(std::shared_ptr<IDigitalBandPassFilter> digitalBandPassFilter) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the current assigned smoothing filter. This function will return a frequency in MHz 
    /// assigned to the filter. This frequency is not actually inherent to the filter, 
    /// but represents the frequency of probes that are best suited for this filter. 
    ///
    /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
    /// or compoundPulseEcho / smoothingFilter
    /// 
    /// @return
    /// The current smoothing filter value.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetSmoothingFilter() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Change the smoothing filter for all beams of this set. The filter set should have a frequency
    /// that is closest to the frequency of the binded probe. The collection of supported smoothing
    /// filters can be accessed with DigitizerTechnology::GetSmoothingFilterSupported.
    /// Will throw if passed smoothingFilter is not from the supported values.
    ///
    /// @param smoothingFilter The desired smoothing filter in MHz.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetSmoothingFilter(double smoothingFilter) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Enable or disable the set smoothing filter for all beams of this set.
    ///
    /// @param doEnable True to enable the smoothing, False to disable.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void EnableSmoothingFilter(bool doEnable) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Check if the set smoothing filter is enabled for all beams of this set.
    ///
    /// @return
    /// True if the smoothing is enabled, False otherwise.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsSmoothingFilterEnabled() const = 0;
  };

  using IFilterSettingsPtr = std::shared_ptr<IFilterSettings>;
  using IFilterSettingsConstPtr = std::shared_ptr<const IFilterSettings>;
}
