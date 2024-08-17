///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IDigitizerTechnologyConfiguration.h.
///
/// Declares the IDigitizerTechnologyConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <ultrasoundConfiguration/IUltrasoundConfiguration.h>
#include <Inspection/ICapabilitiesConfiguration.h>
#include <Inspection/IFullMatrixCaptureConfigurationCollection.h>

namespace Olympus
{
  namespace Inspection
  {
    class IDigitizerTechnologyConfiguration
    {
    public:
      virtual ~IDigitizerTechnologyConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulser voltage
      /// 
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "SectorialPulseEcho" / "pulseVoltage"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "tofd" / "pulseVoltage"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "pulseVoltage"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" / "pulseVoltage"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPitchCatch" / "pulseVoltage"
      ///
      /// @returns The pulser voltage.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetPulserVoltage() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets pulser voltage
      ///
      /// @param voltage The voltage.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPulserVoltage(double voltage) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulser voltages
      ///
      /// @returns The pulser voltages.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::shared_ptr<Instrumentation::IPulserVoltageCollection> GetPulserVoltages() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets band pass filters
      /// 
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "SectorialPulseEcho" / "digitalBandPassFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "tofd" / "digitalBandPassFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "digitalBandPassFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "compoundPulseEcho" / "digitalBandPassFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" / "digitalBandPassFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPitchCatch" / "digitalBandPassFilter"
      ///
      /// @returns The band pass filters.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::shared_ptr<Instrumentation::IDigitalBandPassFilterCollection> GetBandPassFilters() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets smoothing filters
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "SectorialPulseEcho" / "smoothingFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "smoothingFilter"
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "compoundPulseEcho" / "smoothingFilter"
      /// 
      /// @returns The smoothing filters.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::shared_ptr<Instrumentation::ISmoothingFilterCollection> GetSmoothingFilters() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets capabilities configuration
      ///
      /// @returns The capabilities configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICapabilitiesConfigurationPtr GetCapabilitiesConfiguration() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the full matrix capture (FMC) configurations
      ///
      ///  NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture"
      /// 
      /// @returns The full matrix capture (FMC) configurations.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFullMatrixCaptureConfigurationCollectionPtr GetFullMatrixCaptureConfigurations() const = 0;
    };

    using IDigitizerTechnologyConfigurationPtr = std::shared_ptr<IDigitizerTechnologyConfiguration>;
    using IDigitizerTechnologyConfigurationConstPtr = std::shared_ptr<const IDigitizerTechnologyConfiguration>;
  }
}
