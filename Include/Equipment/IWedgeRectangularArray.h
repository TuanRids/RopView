///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment\IWedgeRectangularArray.h.
///
/// NDE file domain path wedges[index] / angleBeamWedge / mountingLocations[index]
/// 
/// Declares the IWedgeRectangularArray interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IProbe.h>
#include <Equipment/IWedgeBlock.h>

namespace Olympus { namespace Equipment {

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Physical wedge on which the phased array transducer in installed.
  /// The information of this section is important to allow RayTracing inside the wedge.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IWedgeRectangularArray : public IWedgeBlock
  {
  public:

    virtual ~IWedgeRectangularArray() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Query if the mounting orientation of the wedge block is supported.
    ///
    /// @param orientation The orientation for the wedge block is either Normal (0 degrees),
    ///  Lateral (90 degrees), ReverseNormal (180 degrees) or ReverseLateral (270 degrees).
    ///
    /// @return true if orientation supported, false if not.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsOrientationSupported(MountingOrientation orientation) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Inserts a supported mounting orientation for the wedge block.
    ///
    /// @param orientation The orientation for the wedge block is either Normal (0 degrees),
    /// Lateral (90 degrees), ReverseNormal (180 degrees) or ReverseLateral (270 degrees).
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void InsertSupportedOrientation(MountingOrientation orientation) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the roof angle of the wedge block in degrees.
    /// 
    /// NDE file domain path wedges[index] / angleBeamWedge / mountingLocations[index] / roofAngle
    ///
    /// @image html RoofAngle.png width=30%
    ///
    /// @return The roof angle of the wedge block in degrees. The roof angle is the angle measured using
    /// the length of the wedge block as a pivot.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetRoof() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a roof angle of the wedge block in degrees.
    ///
    /// @param roof The roof angle of the wedge block in  degrees. The roof angle is the angle measured
    /// using the length of the wedge block as a pivot.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetRoof(double roof) = 0;
  };

  using IWedgeRectangularArrayPtr = std::shared_ptr<IWedgeRectangularArray>;
  using IWedgeRectangularArrayConstPtr = std::shared_ptr<const IWedgeRectangularArray>;
}}
