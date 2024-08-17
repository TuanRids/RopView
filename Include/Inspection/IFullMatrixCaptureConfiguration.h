///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IFullMatrixCaptureConfiguration.h.
///
/// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture"
/// 
/// Declares the IFullMatrixCaptureConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <inspection/IIdentifierCollection.h>

namespace Instrumentation
{
	class IDigitalBandPassFilter;
}


namespace Olympus
{
  namespace Inspection
  {
    /// Values that represent FMC modes (DelayAndSum, PhaseCoherence)
    enum class FmcModeType { DelayAndSum, PhaseCoherence };


    class IFullMatrixCaptureConfiguration
    {
    public:
      virtual ~IFullMatrixCaptureConfiguration() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if the envelope is enabled.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "datasets"[index] 
      /// / "tfm" / signalSource (Analytic: Enabled, Real: Disabled)
      /// 
      /// @returns  True if the envelope is enabled.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsEnvelopeEnabled() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Enables or Disables the envelope.
      ///
      /// @param enable True to enable the envelope, false to disable.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void EnableEnvelope(bool enable) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulse width in nanosecond.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "pulseWidth"
      /// 
      /// @returns The pulse width.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetPulseWidth() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets pulse width in nanosecond.
      ///
      /// @param width The pulse width.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPulseWidth(double width) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulser frequency in hertz.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "pulserFrequency"
      /// 
      /// @returns The pulser frequency.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetPulserFrequency() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets pulser frequency in hertz.
      ///
      /// @param frequency The pulser frequency.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPulserFrequency(double frequency) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulser identifiers.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "pulsers" / "elementId"
      /// 
      /// @returns The pulser identifiers.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IIdentifierCollectionPtr GetPulserIdentifiers() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets receiver identifiers.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "receivers" / "elementId"
      /// 
      /// @returns The receiver identifiers.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IIdentifierCollectionPtr GetReceiverIdentifiers() const = 0;

	  ////////////////////////////////////////////////////////////////////////////////////////////////////
	 /// Get the interface for the digital pass filter linked to this beam set. The interface contains
	 /// the type of filter (LowPass, HighPass, BandPass or none), the high and low cut off frequency
	 /// as well as the characteristic (TOFD or none) of the filter.
	 ///
   /// NDE file domain path TO DO
   /// 
	 /// @return
	 /// The interface IDigitalBandPassFilter for the filter.
	 ////////////////////////////////////////////////////////////////////////////////////////////////////
	  virtual std::shared_ptr<Instrumentation::IDigitalBandPassFilter> GetDigitalBandPassFilter() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets FMC Mode.
    ///
    ///   DelayAndSum,
    ///   PhaseCoherence
    /// 
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "datasets"[index] 
    /// / "tfm" (DelayAndSum) or "pci" (PhaseCoherence)
    /// 
    /// @returns The fmc mode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual FmcModeType GetMode() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets FMC Mode.
    ///
    ///   DelayAndSum,
    ///   PhaseCoherence
    /// 
    /// @param The fmc mode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetMode(FmcModeType mode) = 0;

    };

    using IFullMatrixCaptureConfigurationPtr = std::shared_ptr<IFullMatrixCaptureConfiguration>;
    using IFullMatrixCaptureConfigurationConstPtr = std::shared_ptr<const IFullMatrixCaptureConfiguration>;
	
  }
}
