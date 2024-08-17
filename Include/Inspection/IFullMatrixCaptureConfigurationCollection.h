////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IFullMatrixCaptureConfigurationCollection.h
///
/// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture"
/// 
/// Declares the IFullMatrixCaptureConfigurationCollection interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <Inspection/IFullMatrixCaptureConfiguration.h>

namespace Olympus { 
  namespace Inspection 
  {
    class IFullMatrixCaptureConfigurationCollection
    {
    public:
      virtual ~IFullMatrixCaptureConfigurationCollection() = default;

      ///////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the FMC configurations count
      ///
      /// @returns The count.
      ///////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      ///////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a FMC configuration
      ///
      /// @param index Zero-based.
      ///
      /// @returns The FMC configuration.
      ///////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFullMatrixCaptureConfigurationPtr GetFullMatrixCaptureConfiguration(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds FMC configuration
      ///
      /// @param enableEnvelop The state of the envelop.
      /// @param pulseWidth The pulse width(ns).
      /// @param pulserFrequency The pulser frequency (Hz).
      ///
      /// @returns The FMC configuration added.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFullMatrixCaptureConfigurationPtr Add(bool enableEnvelop, double pulseWidth, double pulserFrequency) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Remove an FMC configuration by giving its index.
      ///
      /// The indexes of the elements after the removed element are all decremented of 1.
      ///
      /// @param index The index of the IFullMatrixCaptureConfiguration desired to be removed.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Remove(std::size_t index) = 0;

	  /////////////////////////////////////////////////////////////////////////////////////////////
	  /// Adds FMC configuration
	  ///
	  /// @param enableEnvelop The state of the envelop.
	  /// @param pulseWidth The pulse width(ns).
	  /// @param pulserFrequency The pulser frequency (Hz).
	  /// @param filter instance of IDigitalBandPassFilter.
	  ///
	  /// @returns The FMC configuration added.
	  /////////////////////////////////////////////////////////////////////////////////////////////
	  virtual IFullMatrixCaptureConfigurationPtr AddEx(bool enableEnvelop, double pulseWidth, double pulserFrequency, std::shared_ptr<Instrumentation::IDigitalBandPassFilter> filter) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Adds FMC configuration
    ///
    /// @param enableEnvelop The state of the envelop.
    /// @param pulseWidth The pulse width(ns).
    /// @param pulserFrequency The pulser frequency (Hz).
    /// @param filter instance of IDigitalBandPassFilter.
    /// @param FMC Mode (DelayAndSum, PhaseCoherence)
    ///
    /// @returns The FMC configuration added.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IFullMatrixCaptureConfigurationPtr AddEx2(bool enableEnvelop, double pulseWidth, double pulserFrequency, std::shared_ptr<Instrumentation::IDigitalBandPassFilter> filter, FmcModeType mode) = 0;

    };

    using IFullMatrixCaptureConfigurationCollectionPtr = std::shared_ptr<IFullMatrixCaptureConfigurationCollection>;
    using IFullMatrixCaptureConfigurationCollectionConstPtr = std::shared_ptr<const IFullMatrixCaptureConfigurationCollection>;
  }
}
