///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICapabilitiesConfiguration.h.
///
/// Declares the ICapabilitiesConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <ultrasoundConfiguration/IDigitalBandPassFilter.h>

namespace Olympus
{
  namespace Inspection
  {
    class ICapabilitiesConfiguration
    {
    public:
      virtual ~ICapabilitiesConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds a digital band pass filter
      ///
      /// If the filter already exists in the digital bandpass filter collection
      /// the return value will be the existing filter and will not be
      /// added in the collection.
      ///
      /// @param type                The type.
      /// @param lowCutOffFrequency  The low cut off frequency.
      /// @param HighCutOffFrequency The high cut off frequency.
      /// @param characteristic      The characteristic.
      ///
      /// @returns A std::shared_ptr&lt;Instrumentation::IDigitalBandPassFilter&gt;
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::shared_ptr<Instrumentation::IDigitalBandPassFilter> AddDigitalBandPassFilter(
        Instrumentation::IDigitalBandPassFilter::Type type,
        double lowCutOffFrequency,
        double HighCutOffFrequency,
        Instrumentation::IDigitalBandPassFilter::Characteristic characteristic) = 0;
    };

    using ICapabilitiesConfigurationPtr = std::shared_ptr<ICapabilitiesConfiguration>;
    using ICapabilitiesConfigurationConstPtr = std::shared_ptr<const ICapabilitiesConfiguration>;
  }
}
