///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IPhasedArrayConfiguration.h.
///
/// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
/// or compoundPulseEcho
/// 
/// Declares the IPhasedArrayConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>

#include <ultrasoundConfiguration/IDigitizingSettings.h>
#include <ultrasoundConfiguration/IPulsingSettings.h>
#include <Inspection/IConfiguration.h>
#include <Inspection/IPhasedArrayBeamConfiguration.h>
#include <Inspection/BeamSet.h>
#include <Inspection/ICalibrationStates.h>
#include <Inspection/IThicknessSettings.h>

namespace Olympus
{
  namespace Inspection
  {
    class IPhasedArrayConfiguration : public IConfiguration
    {
    public:
      virtual ~IPhasedArrayConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets beam count
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / calibrationStates / beams
      /// 
      /// @returns The beam count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetBeamCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a beam
      ///
      /// @param index Zero-based index of the.
      /// 
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / calibrationStates / beams[index]
      ///
      /// @returns The beam.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IPhasedArrayBeamConfigurationPtr GetBeam(std::size_t index) const = 0;

      virtual IPhasedArrayBeamConfigurationPtr AddBeam() = 0;

      virtual void RemoveBeam(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the velocity. The velocity is the sound speed inside the inspected part. The speed may 
      /// be adjusted compared to IMaterial::GetLongitudinalVelocity and IMaterial::GetShearVelocity 
      /// functions and represents the velocity for the wavetype used in the beamset.
      ///
      /// NDE file domain path: dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / velocity
      /// 
      /// NDE file domain path: "dataGroups"[index] / "ultrasound" / "conventionalPulseEcho" of / "velocity"
      /// 
      /// @returns The velocity in meter per seconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetVelocity() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a velocity. The velocity is the sound speed inside the inspected part. The speed may
      /// be adjusted compared to IMaterial::GetLongitudinalVelocity and IMaterial::GetShearVelocity
      /// functions and represents the velocity for the wavetype used in the beamset.
      ///
      /// @param velocity The velocity in meter per seconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetVelocity(double velocity) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets wedge delay. The wedge delay is the constant part of the delay inside the wedge. 
      /// This delay is common to all the beams.
      /// 
      /// @image html WedgeDelay.png width=30%
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / wedgeDelay
      /// 
      /// @returns The wedge delay in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetWedgeDelay() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets wedge delay. The wedge delay is the constant part of the delay inside the wedge. 
      /// This delay is common to all the beams.
      ///
      /// @param wedgeDelay The wedge delay in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetWedgeDelay(double wedgeDelay) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the reference amplitude used for sensitivity calibration. 
      /// Unit is percent between 0 and 100.
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / referenceAmplitude
      /// 
      /// @returns The reference amplitude.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetReferenceAmplitude() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the reference amplitude used for sensitivity calibration. 
      /// Unit is percent between 0 and 100.
      ///
      /// @param referenceAmplitude The reference amplitude.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetReferenceAmplitude(double referenceAmplitude) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the general gain of the beamset
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / gain
      /// 
      /// @returns The gain in dB.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetGain() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the general gain of the beamset
      ///
      /// @param gain The gain in dB.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetGain(double gain) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets pulsing settings
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho
      /// 
      /// @returns The pulsing settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IPulsingSettingsPtr GetPulsingSettings() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets digitizing settings
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho
      /// 
      /// @returns The digitizing settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IDigitizingSettingsPtr GetDigitizingSettings() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the calibration states.
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / calibrationStates
      /// 
      /// @returns The calibration states object.
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
      /// Used by IAcquisitionData::CreateBuffers to create a single Ascan buffer and single
      /// Cscan buffers by gates which have IGateConfiguration::IsReserveCscanBuffer set to true.
      ///
      /// Typically used with a linear formation at zero degree.
      ///
      /// @param merge enable single buffer creation.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void MergeBuffer(bool merge) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if the buffer will be created by beam or not.
      ///
      /// NDE file domain path "dataEncodings"[index] / "discreteGrid" / "storage" / "overwriteMode"
      /// 
      /// @returns True if buffer will be merged, false if not.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsBufferMerged() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the thickness settings for this configuration.
      ///
      /// @returns the thickness settings.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IThicknessSettingsPtr GetThicknessSettings() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the amplitude scaling range percent.
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / amplitudePercentAxis
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
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / sectorialPulseEcho or linearPulseEcho
      /// or compoundPulseEcho / "referenceGain"
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
      /// Gets the application Settings for this configuration.
      ///
      /// NDE file domain path : No more in the setup, All Application Settings needs to be put in HDF5 / Applications node.
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

    using IPhasedArrayConfigurationPtr = std::shared_ptr<IPhasedArrayConfiguration>;
    using IPhasedArrayConfigurationConstPtr = std::shared_ptr<const IPhasedArrayConfiguration>;
  }
}
