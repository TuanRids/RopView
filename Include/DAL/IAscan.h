#pragma once
#include <memory>
#include <string>

#ifdef SWIG
%typemap(cstype) int* "global::System.IntPtr"
%typemap(csout, excode = SWIGEXCODE) int* "{ $excode return $imcall; }"
#endif

namespace Instrumentation
{
  class IDataRange;
  class ISyncSource;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Contains one A-Scan data, information on it's producer and what is needed to interpret it.
  ///
  /// This is an example of how to use the IAscan class.
  /// @snippet PhasedArrayAcquisition/main.cpp Phased Array Acquisition
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IAscan
  {
  public:
    virtual ~IAscan() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// The device Serial number that produced this C-Scan
    ///
    /// @return
    /// Serial number
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::string GetDeviceSerialNumber() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get Beam Firing Order
    ///
    /// @return
    /// Get the index of this A-Scan in the firing sequence on the device given by GetDeviceSerialNumber
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetBeamFiringOrder() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the number of samples there is in this A-Scan
    /// The A-Scan data goes from the GetData return pointer to GetData + GetSampleQuantity
    ///
    /// @return
    /// number of samples
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetSampleQuantity() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get pointer to the beginning of the a-scan
    /// The A-Scan data goes from the GetData return pointer to GetData + GetSampleQuantity
    ///
    /// @return
    /// pointer to the beginning of the a-scan, units of the values is given by GetAmplitudeSamplingDataRange()->GetUnit()
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const int* GetData() const = 0;


    virtual std::shared_ptr<ISyncSource> GetSyncSource() const = 0;
    virtual bool IsSyncedOnSource() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Return the saturation status
    ///
    /// @return
    /// True if the signal is saturated in the A-Scan
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsSaturated() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the range of the A-Scan
    ///
    /// @return
    /// IDataRange object providing the begin and end of the A-Scan, units nanoseconds
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDataRange> GetTimeDataRange() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the min max of the A-Scan Values in converted unit (the whole scale, not in relation to the contained data)
    /// To be used in conjunction with GetAmplitudeSamplingDataRange to convert a-scan data in a usable unit.
    /// IDataRange::GetFloatingMin Gets the minimum value the amplitude can take.
    /// IDataRange::GetFloatingMax Gets the maximum value the amplitude can take.
    ///
    /// @return
    /// IDataRange object providing the minimum and maximum values of the A-Scan, units given by IDataRange::GetUnit()
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDataRange> GetAmplitudeDataRange() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the min max of the A-Scan Values (the whole scale, not in relation to the contained data)
    /// To be used in conjunction with GetAmplitudeDataRange to convert a-scan data in a usable unit.
    /// IDataRange::GetFloatingMin Gets the minimum value the amplitude can take.
    /// IDataRange::GetFloatingMax Gets the maximum value the amplitude can take
    ///
    /// @return
    /// IDataRange object providing the minimum and maximum values of the A-Scan, units given by IDataRange::GetUnit()
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDataRange> GetAmplitudeSamplingDataRange() const = 0;
  };

  using IAscanPtr = std::shared_ptr<IAscan>;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// This is an example of how to use the IAscan class.
/// @example PhasedArrayAcquisition/main.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////
