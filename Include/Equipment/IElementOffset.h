////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IElementOffset.h
///
/// NDE file domain path "wedges"[index] / "angleBeamWedge" / mountingLocations[index]
/// 
/// Declares the ElementOffset structure
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>

namespace Olympus
{
  namespace Equipment
  {
    class IElementOffset;
    using IElementOffsetPtr = std::shared_ptr<IElementOffset>;
    using IElementOffsetConstPtr = std::shared_ptr<const IElementOffset>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Offset of an element, typically the first or the last, in comparison to the probe reference point.
    /// The information of this section is important to allow RayTracing inside the wedge.
    ///
    /// The axis of a probe are defined using the following conventions. The "Wedge Reference Point" is
    /// located at the bottom center of the front wall of the wedge.
    /// @image html AxisDefinition.png width=30%
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class IElementOffset
    {
    public:
      virtual ~IElementOffset() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the primary offset of the 1st transducer element
      /// 
      /// NDE file domain path "wedges"[index] / "angleBeamWedge" / mountingLocations[index] / primaryOffset
      ///
      /// @image html PrimaryOffset.png width=30%
      ///
      /// @returns The primary offset (in millimeter) is the distance between the wedge reference point
      /// and the center of the first element. For a Phased Array wedge the primary axis is at the base
      /// of the wedge in the active direction of the transducer. The exit points of a sectorial scan
      /// would be parallel to the primary axis. The zero of the axis is at the end of the wedge,
      /// opposite to the first element for zero and 90 degree transducer skew. The zero of the
      /// primary axis is opposite to the last element for 180 and 270 transducer skew.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetPrimary() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the primary offset of the 1st transducer element
      ///
      /// @param primary
      /// The primary offset (in millimeter) is the distance between the wedge reference point
      /// and the center of the first element. For a Phased Array wedge the primary axis is at the base
      /// of the wedge in the active direction of the transducer. The exit points of a sectorial scan
      /// would be parallel to the primary axis. The zero of the axis is at the end of the wedge,
      /// opposite to the first element for zero and 90 degree transducer skew. The zero of the
      /// primary axis is opposite to the last element for 180 and 270 transducer skew.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPrimary(double primary) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the secondary offset of the 1st transducer element
      ///
      /// NDE file domain path "wedges"[index] / "angleBeamWedge" / mountingLocations[index] / secondaryOffset
      /// 
      /// @image html SecondaryOffset.png width=30%
      ///
      /// @returns The secondary offset (in millimeter) between the wedge reference point and the center
      /// of the first element. The secondary axis is perpendicular to the primary axis at the base of
      /// the wedge. The zero of the secondary axis is located on the left edge of the wedge when looking
      /// at the face that is the closest to the transducer's first element.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetSecondary() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the secondary offset of the 1st transducer element
      ///
      /// @param secondary
      /// The secondary offset (in millimeter) between the wedge reference point and the center of the first element.
      /// The secondary axis is perpendicular to the primary axis at the base of the wedge. The zero of the
      /// secondary axis is located on the left edge of the wedge when looking at the face that is the closest
      /// to the transducer's first element.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetSecondary(double secondary) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the tertiary offset of the 1st transducer element
      ///
      /// NDE file domain path "wedges"[index] / "angleBeamWedge" / mountingLocations[index] / tertiaryOffset
      /// 
      /// @image html TertiaryOffset.png width=30%
      ///
      /// @returns The tertiary offset in millimeter between the wedge reference point and the first
      /// element. This represents the height of the first element from the wedge base for a transducer
      /// skew of zero degree or the height of the last element for a transducer skew of 180 degree.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetTertiary() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the tertiary offset of the 1st transducer element
      ///
      /// @param tertiary
      /// The tertiary offset in millimeter between the wedge reference point and the first element.
      /// This represents the height of the first element from the wedge base for a transducer skew
      /// of zero degree or the height of the last element for a transducer skew of 180 degree
      ///
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetTertiary(double tertiary) = 0;
    };
  }
}
