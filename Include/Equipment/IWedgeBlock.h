///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment\IWedgeBlock.h.
///
/// NDE file domain path wedges[index]
/// 
/// Declares the IWedgeBlock interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>
#include <Equipment/IWedge.h>


namespace Olympus { namespace Equipment {

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Physical wedge on which the transducer in installed. This block normally has an angle side where
  /// the probe is installed.
  /// The information of this section is important to allow RayTracing inside the wedge.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IWedgeBlock : public IWedge
  {
  public:

    virtual ~IWedgeBlock() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the model of the wedge block.
    ///
    /// NDE file domain path wedges[index] / model
    /// 
    /// @return The model of the wedge block.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const std::wstring& GetModel() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a model for the wedge block.
    ///
    /// @param model The model of the wedge block.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetModel(const std::wstring& model) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the length of the wedge block in millimeters.
    ///
    /// NDE file domain path wedges[index] / angleBeamWedge / length
    /// 
    /// @image html Length.png width=30%
    ///
    /// @return The length of the wedge block in millimeters. The length is the measurement in the
    /// same axis as the transducer's length (Primary Axis), that follows the base of the wedge.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetLength() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a length for the wedge block in millimeters.
    ///
    /// @param length The length of the wedge block in millimeters. The length is the measurement in the
    /// same axis as the transducer's length (Primary Axis), that follows the base of the wedge.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetLength(double length) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the width of the wedge block in millimeters.
    ///
    /// NDE file domain path wedges[index] / angleBeamWedge / width
    /// 
    /// @image html Width.png width=30%
    ///
    /// @return The width of the wedge block in millimeters. The width is the measurement in the
    /// same axis as the transducer's width (Secondary Axis), that follows the base of the wedge.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetWidth() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a width for the wedge block in millimeters.
    ///
    /// @param width The width of the wedge block in millimeters. The width is the measurement in the
    /// same axis as the transducer's width (Secondary Axis), that follows the base of the wedge.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetWidth(double width) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the height of the wedge block in millimeters.
    ///
    /// NDE file domain path wedges[index] / angleBeamWedge / length
    /// 
    /// @image html Height.png width=30%
    ///
    /// @return The height of the wedge block in millimeter. The height is the measurement from the
    /// specimen to the highest point of the wedge block.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetHeight() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a height for the wedge block in millimeters.
    ///
    /// @param height The height of the wedge block in millimeter. The height is the measurement from the
    /// specimen to the highest point of the wedge block.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetHeight(double height) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the angle of the wedge block in degrees.
    ///
    /// NDE file domain path: need to calculatewith wedges[index] / angleBeamWedge / mountingLocations[index] 
    /// / wedgeAngle and velocities
    /// 
    /// @image html WedgeAngle.png width=30%
    ///
    /// @return The angle of the wedge block in degree between the horizontal and the diagonal section of
    /// the weld.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetAngle() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets an angle for the wedge block in degrees.
    ///
    /// @param angle The angle of the wedge block in degree between the horizontal and the diagonal section
    /// of the weld.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetAngle(double angle) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the velocity of the wedge block material in m/s.
    ///
    /// NDE file domain path wedges[index] / angleBeamWedge / longitudinalVelocity
    /// 
    /// @return Velocity of the wedge block material in m/s.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetMaterialVelocity() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a velocity for the wedge block material in m/s
    ///
    /// @param materialVelocity Velocity of the wedge block material in m/s.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetMaterialVelocity(double materialVelocity) = 0;
  };

  using IWedgeBlockPtr = std::shared_ptr<IWedgeBlock>;
  using IWedgeBlockConstPtr = std::shared_ptr<const IWedgeBlock>;
}}
