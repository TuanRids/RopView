////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IUltrasoundConfiguration.h
///
/// Declares the ultrasoundConfiguration interface.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <instrumentation/IAcquisitionConfiguration.h>
#include <instrumentation/IAcquisitionConfigurationCollection.h>
#include <ultrasoundConfiguration/IAdjustedParameter.h>
#include <ultrasoundConfiguration/IAdjustedParameterCollection.h>
#include <ultrasoundConfiguration/IAmplitudeSettings.h>
#include <ultrasoundConfiguration/IBeam.h>
#include <ultrasoundConfiguration/IBeamFormation.h>
#include <ultrasoundConfiguration/IBeamFormationCollection.h>
#include <ultrasoundConfiguration/IBeamSet.h>
#include <ultrasoundConfiguration/IBeamSetFactory.h>
#include <ultrasoundConfiguration/IConnector.h>
#include <ultrasoundConfiguration/IConnectorCollection.h>
#include <ultrasoundConfiguration/IDeviceCapabilities.h>
#include <instrumentation/IDeviceConfiguration.h>
#include <instrumentation/IDeviceConfigurationCollection.h>
#include <ultrasoundConfiguration/IDigitalBandPassFilter.h>
#include <ultrasoundConfiguration/IDigitalBandPassFilterCollection.h>
#include <ultrasoundConfiguration/IDigitizerCapabilities.h>
#include <ultrasoundConfiguration/IDigitizerTechnology.h>
#include <ultrasoundConfiguration/IDigitizingSettings.h>
#include <ultrasoundConfiguration/IElementDelay.h>
#include <ultrasoundConfiguration/IElementDelayCollection.h>
#include <instrumentation/IEncoder.h>
#include <instrumentation/IEncoderCollection.h>
#include <ultrasoundConfiguration/IFilterSettings.h>
#include <ultrasoundConfiguration/IFiringBeam.h>
#include <ultrasoundConfiguration/IFiringBeamSet.h>
#include <ultrasoundConfiguration/IFiringBeamSetCollection.h>
#include <ultrasoundConfiguration/IGate.h>
#include <ultrasoundConfiguration/IGateCollection.h>
#include <instrumentation/IGpioConnector.h>
#include <instrumentation/IGpioConnectorCollection.h>
#include <ultrasoundConfiguration/IPulserVoltageCollection.h>
#include <ultrasoundConfiguration/IPulsingSettings.h>
#include <ultrasoundConfiguration/ISmoothingFilterCollection.h>
#include <ultrasoundConfiguration/ITcg.h>
#include <ultrasoundConfiguration/ITcgPoint.h>
#include <ultrasoundConfiguration/ITcgPointCollection.h>
#include <ultrasoundConfiguration/ITimeSettings.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include <instrumentation/IDigitalInputPin.h>
#include <instrumentation/IDigitalInputPinCollection.h>


namespace Instrumentation
{
  class IFiringBeamSetCollection;
  class IDigitizerTechnology;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Contains all the information regarding the configuration of Ultrasound parameters for the 
  /// corresponding device.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IUltrasoundConfiguration
  {
  public:
    enum class RecurrenceMode { Automatic, Manual };

    virtual ~IUltrasoundConfiguration() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the interface IDigitizerTechnology that is used to get access to the digitizer
    /// connectors and capabilities for the given technology.
    ///
    /// @param technology The desired ultrasound technology of the digitizer.
    ///
    /// @return
    /// The interface IDigitizerTechnology.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDigitizerTechnology> GetDigitizerTechnology(const UltrasoundTechnology& technology) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the interface IFiringBeamSetCollection that is used to fire sets of Beams from IBeamSet on connectors
    /// that were fetched with GetConventional and GetPhasedArray.
    ///
    /// @return
    /// The interface IFiringBeamSetCollection.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IFiringBeamSetCollection> GetFiringBeamSetCollection() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the mode of recurrence : Manual or Automatic. If the mode is automatic, the software
    /// will divide the firing period of the beam set equally between all its beams. In manual mode,
    /// each beam can have a specified digitizing period by using IBeam::SetRecurrence.
    //TODO: PBLTODO Change name after PRF discussion.
    ///
    /// @return
    /// The recurrence mode enum.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual RecurrenceMode GetRecurrenceMode() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the mode of recurrence : Manual or Automatic. If the mode is automatic, the software
    /// will divide the firing period of the beam set equally between all its beams. In manual mode,
    /// each beam can have a specified digitizing period by using IBeam::SetRecurrence.
    ///
    /// @param mode The desired recurrence mode enum.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetRecurrenceMode(RecurrenceMode mode) = 0;

    virtual bool IsTechnologySupported(UltrasoundTechnology technology) const = 0;
  };

  using IUltrasoundConfigurationPtr = std::shared_ptr<IUltrasoundConfiguration>;
  using IUltrasoundConfigurationConstPtr = std::shared_ptr<const IUltrasoundConfiguration>;
}
