///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment\IWedgeDualRectangularArray.h.
///
/// NDE file domain path wedges[index] / angleBeamWedge / mountingLocations[index]
/// 
/// Declares the IWedgeDualRectangularArray interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IWedgeRectangularArray.h>

namespace Olympus {
  namespace Equipment {

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Physical wedges on which the phased array transducers are installed. The parameters of the base
  /// class are used for both wedges, either by mirroring or by duplicating.
  /// The information of this section is important to allow RayTracing inside the wedge.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IWedgeDualRectangularArray : public IWedgeRectangularArray
  {
  public:
    virtual ~IWedgeDualRectangularArray() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets squint angle of the wedge block in degrees.
    /// 
    /// NDE file domain path wedges[index] / angleBeamWedge / mountingLocations[index] / squintAngle
    ///
    /// @image html Squint.png width=30%
    ///
    /// @return
    /// The squint angle is half the angle between the primary axis of transmitter and receiver arrays.
    /// A symmetrical rotation is automatically applied to the receiving array. The squint angle can
    /// have values between -89.9deg and 89.9deg, and a positive squint angle means that the primary axes
    /// of the arrays cross in front of the arrays.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetSquintAngle() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets squint angle of the wedge block in degrees.
    ///
    /// @param squintAngle
    /// The squint angle is half the angle between the primary axis of transmitter and receiver arrays.
    /// A symmetrical rotation is automatically applied to the receiving array. The squint angle can
    /// have values between -89.9deg and 89.9deg, and a positive squint angle means that the primary axes
    /// of the arrays cross in front of the arrays.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetSquintAngle(double squintAngle) = 0;
  };

  using IWedgeDualRectangularArrayPtr = std::shared_ptr<IWedgeDualRectangularArray>;
  using IWedgeDualRectangularArrayConstPtr = std::shared_ptr<const IWedgeDualRectangularArray>;
}}