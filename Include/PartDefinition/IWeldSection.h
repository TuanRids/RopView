///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\IWeldSection.h.
///
/// NDE file domain path specimens[index] / weldGeometry /
/// upperCap or lowerCap or fills[index] or hotPass or land or root
/// 
/// Declares the IWeldSection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus {
  namespace PartDefinition
  {
    class IWeldSection
    {
    public:
      virtual ~IWeldSection() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the height of the weld section
      ///
      ///  NDE file domain path specimens[index] / weldGeometry / SECTION_NAME / height
      /// 
      /// @return
      /// The height of the weld section in millimeter
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetHeight() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the height of the weld section
      ///
      /// @param height
      /// The height of the weld section in millimeter
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetHeight(double height) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the angle of the weld section
      ///
      /// NDE file domain path specimens[index] / weldGeometry / SECTION_NAME / angle
      /// 
      /// @return
      /// The angle of the weld section in degree. When looking at the side cut view of the weld, the angle
      /// is between the vertical and the weld side.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetAngle() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the angle of the weld section
      ///
      /// @param angle
      /// The angle of the weld section in degree. When looking at the side view of the weld, the angle
      /// is between the vertical line and the weld side.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAngle(double angle) = 0;
    };

    using IWeldSectionPtr = std::shared_ptr<IWeldSection>;
    using IWeldSectionConstPtr = std::shared_ptr<const IWeldSection>;
  }
}
