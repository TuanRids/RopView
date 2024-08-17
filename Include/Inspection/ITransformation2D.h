////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ITransformation2D.h
///
/// NDE file domain path wedges[index] / positioning
/// 
/// Declares the translation/rotation of a probe on the Specimen
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>

namespace Olympus
{
  namespace Inspection
  {
    class ITransformation2D;
    using ITransformation2DPtr = std::shared_ptr<ITransformation2D>;
    using ITransformation2DConstPtr = std::shared_ptr<const ITransformation2D>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Translates and rotates a probe from the specimen origin. For a plate, assume a cube sitting on
    /// a surface in front of you. The front surface is seen directly and the top surface will be inspected.
    /// The origin of the axis system is at the top left of the front surface. From there, the Y axis goes
    /// to the right, the X axis goes to the back and the Z axis goes to the bottom. The Z axis is the ultrasound
    /// axis. 
    ///
    /// For a cylinder (pipe or a bar), assume it is laying in front of you stretching from left to right.
    /// The origin of the axis is at the top-left of the cylinder. The X axis goes toward the back of the 
    /// pipe, around the circumference. The Y axis goes to the right and the Z axis goes inside the cylinder. 
    /// The Z axis is the ultrasound axis. 
    /// 
    /// Before the translation the probe sits at the origin of the specimen axis with the X axis of the 
    /// specimen aligned to the primary axis of the probe.
    /// 
    /// @image html Transform2DReferential.png width=75%
    /// 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class ITransformation2D
    {
    public:
      virtual ~ITransformation2D() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the translation on the X axis, in mm
      ///
      /// NDE file domain path wedges[index] / positioning / uCoordinateOffset
      /// 
      /// @return 
      /// The translation of the probe on the X axis. A positive translation P will move the probe from zero 
      /// to P on the X axis.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetX() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the translation on the X axis, in mm
      ///
      /// NDE file domain path wedges[index] / positioning / vCoordinateOffset
      /// 
      /// @param x The translation of the probe on the X axis. A positive translation P will move the probe from zero 
      /// to P on the X axis.
      ///
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetX(double x) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the translation on the Y axis, in mm
      ///
      /// @return 
      /// The translation of the probe on the Y axis. A positive translation P will move the probe from zero 
      /// to P on the Y axis.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetY() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the translation on the Y axis, in mm
      ///
      /// @param y The translation of the probe on the Y axis. A positive translation P will move the probe from zero 
      /// to P on the Y axis.
      ///
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetY(double y) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the rotation angle of the probe
      ///
      /// NDE file domain path wedges[index] / positioning / skewAngle
      /// 
      /// @return 
      /// The probe rotataion in degree. The rotation must be a multiple of 90 and from 0 to 270 and is 
      /// clockwise. 
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetAngle() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the rotation angle of the probe
      ///
      /// @param angle The probe rotataion in degree. The rotation must be a multiple of 90 and from 0 to 270 and is 
      /// clockwise. 
      ///
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAngle(double angle) = 0;
    };
  }
}
