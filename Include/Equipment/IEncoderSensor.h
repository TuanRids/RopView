///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment/IEncoderSensor.h.
///
/// Declares the IEncoderSensor interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>

namespace Olympus
{
  namespace Equipment
  {
    class IPin;
    using IPinPtr = std::shared_ptr<IPin>;

    class IEncoderSensor
    {
    public:
      virtual ~IEncoderSensor() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the name of this encoder
      ///
      /// 
      /// 
      /// @returns The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets distance resolution
      ///
      /// NDE file domain path "motionDevices"[index] / "quadratureEncoder" / "resolution"
      /// 
      /// @returns The distance resolution in step/mm.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetDistanceResolution() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets distance resolution
      ///
      /// NDE file domain path "motionDevices"[index] / "quadratureEncoder" / "resolution"
      /// 
      /// @param  resolution  The resolution.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetDistanceResolution(double resolution) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets phase A/clock/up/down pin
      ///
      /// Setters allow nullptr to be passed as argument so that the encoder can be
      /// connected to another device or not connected on this pin
      ///
      /// @returns The phase a pin.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IPinPtr GetPhaseAPin() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets phase A/clock/up/down pin
      ///
      /// @param phaseAPin The phase a pin.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPhaseAPin(IPinPtr phaseAPin) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets phase B/direction pin
      ///
      /// @returns The phase b pin.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IPinPtr GetPhaseBPin() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets phase B/direction pin
      ///
      /// @param phaseBPin The phase b pin.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPhaseBPin(IPinPtr phaseBPin) = 0;
    };

    using IEncoderSensorPtr = std::shared_ptr<IEncoderSensor>;
    using IEncoderSensorConstPtr = std::shared_ptr<const IEncoderSensor>;
  }
}
