///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IUltrasoundDigitizerConfiguration.h.
///
/// Declares the IUltrasoundDigitizerConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include <ultrasoundConfiguration/IUltrasoundConfiguration.h>
#include <Inspection/IDigitizerTechnologyConfiguration.h>

namespace Olympus
{
  namespace Inspection
  {
    class IUltrasoundDigitizerConfiguration
    {
    public:
      virtual ~IUltrasoundDigitizerConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets recurrence mode
      ///
      /// @returns The recurrence mode.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IUltrasoundConfiguration::RecurrenceMode GetRecurrenceMode() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets recurrence mode
      ///
      /// @param mode The mode.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetRecurrenceMode(Instrumentation::IUltrasoundConfiguration::RecurrenceMode mode) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if 'technology' is technology supported
      ///
      /// @param technology The technology.
      ///
      /// @returns True if technology supported, false if not.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsTechnologySupported(Instrumentation::UltrasoundTechnology technology) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets digitizer technology configuration
      ///
      /// @param technology The technology.
      ///
      /// @returns The digitizer technology configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IDigitizerTechnologyConfigurationPtr
        GetDigitizerTechnologyConfiguration(Instrumentation::UltrasoundTechnology technology) const = 0;
    };

    using IUltrasoundDigitizerConfigurationPtr = std::shared_ptr<IUltrasoundDigitizerConfiguration>;
    using IUltrasoundDigitizerConfigurationConstPtr = std::shared_ptr<const IUltrasoundDigitizerConfiguration>;
  }
}
