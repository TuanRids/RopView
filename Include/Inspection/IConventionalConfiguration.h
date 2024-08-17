///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IConventionalConfiguration.h.
///
/// Declares the IConventionalConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>

#include <ultrasoundConfiguration/IDigitizingSettings.h>
#include <ultrasoundConfiguration/IPulsingSettings.h>
#include <ultrasoundConfiguration/ITcg.h>
#include <Inspection/InspectionEnums.h>
#include <Inspection/IConfiguration.h>
#include <Inspection/ICalibrationStates.h>
#include <Inspection/IThicknessSettings.h>

namespace Olympus
{
  namespace Inspection
  {
    class IGateConfigurationCollection;
    using IGateConfigurationCollectionPtr = std::shared_ptr<IGateConfigurationCollection>;
    using IGateConfigurationCollectionConstPtr = std::shared_ptr<const IGateConfigurationCollection>;

    class IConventionalConfiguration : public IConfiguration
    {
    public:
      virtual ~IConventionalConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a velocity
      /// 
      /// @param velocity The velocity in meter per seconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetVelocity(double velocity) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the velocity
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "velocity"
      /// 
      /// @returns The velocity in meter per seconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetVelocity() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets wedge delay. The wedge delay is the constant part of the delay inside the wedge. 
      /// This delay is common to all the beams.
      /// 
      /// @param wedgeDelay The wedge delay in Nanoseconds..
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetWedgeDelay(double wedgeDelay) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets wedge delay. The wedge delay is the constant part of the delay inside the wedge.
      /// This delay is common to all the beams.
      /// 
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "wedgeDelay"
      /// 
      /// @image html WedgeDelay.png width=30%
      ///
      /// @returns The wedge delay in Nanoseconds..
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetWedgeDelay() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the digitizing delay.
      /// 
      /// @param digitizingDelay The digitizing delay in Nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetDigitizingDelay(double digitizingDelay) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets digitizing delay, which is the time difference in nanoseconds between the specimen 
      /// front wall and the beginning of the A-scan digitizing. 
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "ultrasoundAxis" / "ascanLength"
      /// 
      /// @returns The digitizing delay in Nanoseconds..
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetDigitizingDelay() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets exit point
      ///
      /// @param exitPoint The exit point in millimeters.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetExitPoint(double exitPoint) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets exit point
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / xImagingExitPoint
      /// 
      /// @returns The exit point in millimeters.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetExitPoint() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets firing order
      ///
      /// @param firingOrder The firing order.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetFiringOrder(std::size_t firingOrder) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets firing order
      ///
      /// NDE file domain path Not Available in NDE file format
      /// 
      /// @returns The firing order.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetFiringOrder() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Sets the reference amplitude used for sensitivity calibration. 
      /// Unit is percent between 0 and 100.
      /// 
      /// @param referenceAmplitude The reference amplitude.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetReferenceAmplitude(double referenceAmplitude) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the reference amplitude used for sensitivity calibration. 
      /// Unit is percent between 0 and 100.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "referenceAmplitude"
      /// 
      /// @returns The reference amplitude.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetReferenceAmplitude() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a gain
      /// 
      /// @param gain The gain in dB.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetGain(double gain) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the gain
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "gain"
      /// 
      /// @returns The gain in dB.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetGain() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the digitizing length
      ///
      /// @param digitizingLength Digitizing length in nanoseconds
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetDigitizingLength(double digitizingLength) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the digitizing length
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "ultrasoundAxis" / "ascanLength"
      /// 
      /// @returns The digitizing length in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetDigitizingLength() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a recurrence
      ///
      /// @param recurence The recurence in Nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetRecurrence(double recurence) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the recurrence
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "recurrence"
      /// 
      /// @returns The recurrence in Nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetRecurrence() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets digitizing settings
      ///
      /// @returns The digitizing settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IDigitizingSettingsPtr GetDigitizingSettings() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulsing settings
      ///
      /// @returns The pulsing settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IPulsingSettingsPtr GetPulsingSettings() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets gate configurations
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "ultrasoundAxis" / gates
      /// 
      /// @returns The gate configurations.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IGateConfigurationCollectionPtr GetGateConfigurations() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the tcg
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "ultrasoundAxis" / "tcg"
      /// 
      /// @returns The tcg.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::ITcgPtr GetTcg() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the refracted angle of the beam in the specimen.
      ///
      /// NDE file domain path: wedges[index] / angleBeamWedge / mountingLocations[index] / wedgeAngle
      /// 
      /// @returns The the refracted angle.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetRefractedAngle() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the refracted angle of the beam in the specimen.
      ///
      /// @param refractedAngle The the refracted angle.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetRefractedAngle(double refractedAngle) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the calibration states.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "ultrasoundAxis" / "calibrationStates"
      /// 
      /// @returns The the refracted angle.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICalibrationStatesPtr GetCalibrationStates() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the amplitude scaling range percent.
      ///
      /// @returns the amplitude scaling range.
      /////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(suppress : 4996)
      __declspec(deprecated("Deprecated. Use GetAmplitudeScalingRangeEx")) virtual AmplitudeScalingRange GetAmplitudeScalingRange() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the amplitude scaling range percent.
      ///
      /// @param amplitudeScalingRange The percent range.
      /////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(suppress : 4996)
      __declspec(deprecated("Deprecated. Use GetScanEntryPointEx")) virtual void SetAmplitudeScalingRange(AmplitudeScalingRange amplitudeScalingRange) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the amplitude scaling range percent.
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / amplitudePercentAxis
      /// 
      /// @returns the amplitude scaling range.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetAmplitudeScalingRangeEx() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the amplitude scaling range percent.
      ///
      /// @param amplitudeScalingRange The percent range.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAmplitudeScalingRangeEx(double amplitudeScalingRange) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a analysis gain
      /// Analysis gain may have been applied or not to Ascan and Cscan data.
      /// You must set the corresponding multiplier factor with SetAscanDataAmplitudeMultiplierFactor.
      /// This multiplier factor will be available with data descriptor using GetAmplitudeMultiplierFactor.
      ///
      /// @param gain The analysis gain in dB.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAnalysisGain(double gain) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the analysis gain
      /// Analysis gain may have been applied or not to Ascan and Cscan data.
      /// Use GetAscanDataAmplitudeMultiplierFactor to know if data must be adjusted.
      /// Multiplier factor is also available with data descriptor using GetAmplitudeMultiplierFactor.
      ///
      /// NDE file domain path  processes[index] / analysisBeamGain / gains[index]
      /// 
      /// @returns The analysis gain in dB.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetAnalysisGain() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the Ascan data amplitude multiplier factor
      /// Analysis gain may have been applied or not to Ascan and Cscan data.
      /// Use this call to know if data must be adjusted.
      /// Multiplier factor is also available with data descriptor using GetAmplitudeMultiplierFactor.
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / datasets[index] / ascan / amplitudeSamplingAxis
      /// 
      /// @returns The Ascan data amplitude multiplier factor.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetAscanDataAmplitudeMultiplierFactor() const = 0;
      
      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a Ascan data amplitude multiplier factor
      /// This value is in relation with the analysis gain.
      /// You must set the corresponding multiplier factor if Ascan data must be adjusted.
      /// Amplitude ratio example: 6 dB is 10^(6/20) = 1.9952
      ///
      /// @param multiplierFactor The Ascan data amplitude multiplier factor.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAscanDataAmplitudeMultiplierFactor(double multiplierFactor) = 0;

      ///////////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if a reference gain is used.
      /// 
      /// @returns True if a reference gain exist, false if not.
      ///////////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool HasReferenceGain() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Enables or Disables the reference gain usage.
      ///
      /// @param hasRefGain True if reference gain value is used, false if not.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void HasReferenceGain(bool hasRefGain) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the reference gain used in dB.
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" or
      /// conventionalPitchCatch or tofd / "referenceGain"
      /// 
      /// @returns The reference gain.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetReferenceGain() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the reference gain used in dB.
      ///
      /// @param refGain The reference gain.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetReferenceGain(double refGain) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the thickness settings for this configuration.
      ///
      /// NDE file domain path Not Available in NDE file format
      /// 
      /// @returns the thickness settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IThicknessSettingsPtr GetThicknessSettings() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the application Settings for this configuration.
      ///
      ///  NDE file domain path : No more in the setup, All Application Settings needs to be put in HDF5 / Applications node.
      /// 
      /// @returns the application settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::string GetApplicationSettings(std::string application) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the application Settings for this configuration.
      ///
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetApplicationSettings(std::string json) = 0;
    };

    using IConventionalConfigurationPtr = std::shared_ptr<IConventionalConfiguration>;
    using IConventionalConfigurationConstPtr = std::shared_ptr<const IConventionalConfiguration>;
  }
}
