///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICalibrationStates.h.
///
/// Declares the ICalibrationStates interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus
{
  namespace Inspection
  {
    class ICalibrationStates
    {
    public:
      virtual ~ICalibrationStates() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// True if the velicity calibration has been done on the aquired data
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho or other" / "calibrationStates"[index] / "velocityCalibration" / "calibrated"
      /// 
      /// @returns True if calibrated.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsVelocityCalibrated() const = 0;
      virtual void VelocityCalibrated(bool) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// True if the wedge delay calibration has been done on the aquired data
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho or other" / "calibrationStates"[index] / "wedgeDelayCalibration" / "calibrated"
      /// 
      /// @returns True if calibrated.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsWedgeDelayCalibrated() const = 0;
      virtual void WedgeDelayCalibrated(bool) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// True if the sensitivity calibration has been done on the aquired data
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho or other" / "calibrationStates"[index] / "sensitivityCalibration" / "calibrated"
      /// 
      /// @returns True if calibrated.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsSensitivityCalibrated() const = 0;
      virtual void SensitivityCalibrated(bool) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// True if the TCG calibration has been done on the aquired data
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho or other" / "calibrationStates"[index] / "tcgCalibration" / "calibrated"
      /// 
      /// @returns True if calibrated.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsTCGCalibrated() const = 0;
      virtual void TCGCalibrated(bool) = 0;
    };

    using ICalibrationStatesPtr = std::shared_ptr<ICalibrationStates>;
    using ICalibrationStatesConstPtr = std::shared_ptr<const ICalibrationStates>;
  }
}

