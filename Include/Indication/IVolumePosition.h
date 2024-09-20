///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Indication\IVolumePosition.h.
///
/// NDE file domain path / The custom section are not part of the Domain, 
/// 
/// Declares the IVolumePosition interface
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <Indication/IPatchPosition.h>

namespace Olympus
{
  namespace Indication
  {
    /// A volume position.
    class IVolumePosition : public IPatchPosition
    {
    public:
      virtual ~IVolumePosition() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the ultrasound position
      ///
      /// @returns The ultrasound position in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetUltrasoundPosition() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the ultrasound position
      ///
      /// @param ultrasoundPosition The ultrasound position in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetUltrasoundPosition(double ultrasoundPosition) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the amplitude
      ///
      /// @returns The amplitude.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetAmplitude() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets an amplitude
      ///
      /// @param amplitude The amplitude.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAmplitude(double amplitude) = 0;
    };

    using IVolumePositionPtr = std::shared_ptr<IVolumePosition>;
    using IVolumePositionConstPtr = std::shared_ptr<const IVolumePosition>;
  }
}