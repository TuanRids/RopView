///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IEncoderConfiguration.h.
///
/// Declares the IEncoderConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>
#include <Equipment/IPin.h>

namespace Olympus
{
  namespace Inspection
  {
    /// Values that represent axis types
    enum class AxisType {
      Scan,
      Index
    };

    /// Values that represent encoder types
    enum class EncoderType {
      ClockDir, Quadrature, PulseUp, PulseDown
    };

    class IEncoderConfiguration
    {
    public:
      virtual ~IEncoderConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the name of the encoder
      ///
      /// NDE file domain path "dataEncodings"[index] / "discreteGrid" / "oneLineScan" / "uCoordinateAxis" || "vCoordinateAxis" / "name"
      /// NDE file domain path "dataEncodings"[index] / "discreteGrid" / "rasterScan" / "uCoordinateAxis" || "vCoordinateAxis" / "name"
      /// 
      /// @returns The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the distance resolution of the encoder in step/mm
      ///
      /// NDE file domain path "dataEncodings"[index] / "discreteGrid" / "oneLineScan" / "uCoordinateAxis" || "vCoordinateAxis" / "resolution"
      /// NDE file domain path "dataEncodings"[index] / "discreteGrid" / "rasterScan" / "uCoordinateAxis" || "vCoordinateAxis" / "resolution"
      /// 
      /// @returns The distance resolution of the encoder.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetDistanceResolution() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the distance resolution of the encoder
      ///
      /// @param resolution The resolution of the encoder in step/mm
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetDistanceResolution(double resolution) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the firing resolution of the encoder in step/pulse
      ///
      /// NDE file domain path Not available in NDE format
      /// 
      /// @returns The firing resolution of the encoder.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetFiringResolution() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the firing resolution of the encoder
      ///
      /// @param resolution The resolution of the encoder in step/pulse
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetFiringResolution(double resolution) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the type of the encoder
      ///
      /// NDE file domain path "motionDevices"[index] / "quadratureEncoder" or "paceBasedEncoding" 
      /// 
      /// @returns The type of the encoder.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual EncoderType GetType() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a type of the encoder
      ///
      /// @param encoderType Type of the encoder.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetType(EncoderType encoderType) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets preset value of the encoder. This correspond to the distance value of the encoder
      /// on a reset event
      ///
      /// NDE file domain path Not available in NDE format
      /// 
      /// @returns The preset value of the encoder.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetPresetValue() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets preset value of the encoder. This correspond to the distance value of the encoder
      /// on a reset event
      ///
      /// @param preset The preset of the encoder.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPresetValue(std::size_t preset) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if the encoder is inverted. If true, a positive count will move in the negative
      /// direction
      ///
      /// NDE file domain path Not available in NDE format
      /// 
      /// @returns True if inverted, false if not.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsInverted() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the IsInverted member of the encoder. If true, a positive count will move in the
      /// negative direction
      ///
      /// @param isInverted True if is inverted, false if not.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Invert(bool isInverted) = 0;
    };

    using IEncoderConfigurationPtr = std::shared_ptr<IEncoderConfiguration>;
    using IEncoderConfigurationConstPtr = std::shared_ptr<const IEncoderConfiguration>;
  }
}
