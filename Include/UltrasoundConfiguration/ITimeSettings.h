////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ITimeSettings.h
///
/// NDE file domain path  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho
/// or compoundPulseEcho
/// 
/// Declares the ITimeSettings interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IAdjustedParameterCollection;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Controls time related settings, more precisely the Ascan's compression factor, 
  /// the synchronization mode, the sampling decimation factor and the sampling resolution. 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class ITimeSettings
  {
  public:
    enum class SamplingDecimationFactor { One = 1, Two = 2, Four = 4, Eight = 8, Sixteen = 16, ThirtyTwo = 32, SixtyFour = 64 };
    enum class AscanSynchroMode { Absolute, RelativeGateSynchro };
    enum class TCGSynchroMode { RelativeAscanSynchro, RelativeGateSynchro };
    enum class CscanInterpolationFactor { One = 1, Four = 4 };


    virtual ~ITimeSettings() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the compression factor. This parameter, with the digitizing frequency, influence the 
    /// AScan resolution, i.e. how many nanoseconds each data points covers.
    /// 
    /// NDE file domain path:  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho 
    /// or compoundPulseEcho / ascanCompressionFactor
    /// 
    /// @return
    /// The compression factor.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual int GetAscanCompressionFactor() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the compression factor. This parameter, with the digitizing frequency, influence the 
    /// AScan resolution, i.e. how many nanoseconds each data points represents.
    ///
    /// @param compressionFactor The desired compression.
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetAscanCompressionFactor(int compressionFactor) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the synchronization mode for the AScan : Absolute or RelativeGateSynchro.
    /// In RelativeGateSynchro the start of the AScan will be synced with the events of Gate I. 
    ///
    /// NDE file domain path:  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho 
    /// or compoundPulseEcho / ascanSynchroMode
    /// 
    /// @return
    /// The synchro mode enum.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual AscanSynchroMode GetAscanSynchroMode() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the synchronization mode for the AScan : Absolute or RelativeGateSynchro.
    /// In RelativeGateSynchro the start of the AScan will be synced with the events of Gate I. 
    ///
    /// @param ascanSynchroMode The synchro mode enum.
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetAscanSynchroMode(AscanSynchroMode ascanSynchroMode) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the sampling decimation factor: One, Two, Four, Eight, Sixteen, ThirtyTwo, SixtyFour
    /// The sampling decimation factor divides the 100Mhz base frequency to set the digitizing frequency.
    /// The digitalizing frequency controls the rate at which data is converted from
    /// analog to digital. A higher frequency means that less time separates each data point.
    /// The digitizing frequency and the compression factor directly impact the AScan resolution.
    ///
    /// NDE file domain path: Not available
    /// 
    /// @return
    /// The sampling decimation factor enum.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual SamplingDecimationFactor GetSamplingDecimationFactor() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the sampling decimation factor: One, Two, Four, Eight, Sixteen, ThirtyTwo, SixtyFour
    /// The sampling decimation factor divides the 100Mhz base frequency to set the digitizing frequency.
    /// The digitizing frequency controls the rate at which data is converted from
    /// analog to digital. A higher frequency means that less time separates each data point.
    /// The digitizing frequency and the compression factor directly impact the AScan resolution.
    ///
    /// @param samplingDecimationFactor The desired sampling decimation factor enum.
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetSamplingDecimationFactor(SamplingDecimationFactor samplingDecimationFactor) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the sampling resolution of the AScan. The resolution depends on the compression factor
    /// and the digitizing frequency.
    ///
    /// @return
    /// The sampling resolution.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual NanoSecond GetAscanSamplingResolution() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the synchronization mode for the TCG : RelativeAscanSynchro or RelativeGateSynchro.
    /// In RelativeAscanSynchro the TCG will follow the Ascan Synchro mode configuration. (DEFAULT)
    /// In RelativeGateSynchro the start of the TCG will be synced with the events of Gate I. 
    ///
    /// NDE file domain path:  dataGroups[index] / ultrasound / sectorialPulseEcho or linearPulseEcho 
    /// or compoundPulseEcho / tcgSynchroMode
    /// 
    /// @return
    /// The TCGSynchroMode mode enum.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual TCGSynchroMode GetTCGSynchroMode() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the synchronization mode for the TCG : RelativeAscanSynchro or RelativeGateSynchro.
    /// In RelativeAscanSynchro the TCG will follow the Ascan Synchro mode configuration. (DEFAULT)
    /// In RelativeGateSynchro the start of the TCG will be synced with the events of Gate I. 
    ///
    /// @param tcgSynchroMode The TCGSynchroMode mode enum.
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetTCGSynchroMode(TCGSynchroMode tcgSynchroMode) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the 
    /// 
    /// NDE file domain path: Not available
    /// 
    /// @return
    /// The 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CscanInterpolationFactor GetCscanInterpolationFactor() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set 
    /// @param 
    ///
    /// @return
    /// The 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetCscanInterpolationFactor(CscanInterpolationFactor cscanInterpolationFactor) = 0;
  };

  using ITimeSettingsPtr = std::shared_ptr<ITimeSettings>;
  using ITimeSettingsConstPtr = std::shared_ptr<const ITimeSettings>;
}
