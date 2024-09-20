///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\IWeldLandOfCrossPenetration.h.
///
/// NDE file domain path specimens[index] / weldGeometry / land
/// 
/// Declares the IWeldLandOfCrossPenetration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus {
  namespace PartDefinition
  {
    class IWeldLandOfCrossPenetration
    {
    public:
      virtual ~IWeldLandOfCrossPenetration() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the height of the land section
      ///
      /// NDE file domain path specimens[index] / weldGeometry / land / height
      /// 
      /// @return
      /// The height of the land section in millimeters.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetHeight() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the height of the land section
      ///
      /// @param height
      /// The height of the land section in millimeters.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetHeight(double height) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the width of the land section
      ///
      /// NDE file domain path specimens[index] / weldGeometry / offset
      /// 
      /// @return
      /// The width of the land section in millimeters measured from the center to the side.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetWidth() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the width of the land section
      ///
      /// @param width
      /// The width of the land section in millimeters measured from the center to the side.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetWidth(double width) = 0;
    };

    using IWeldLandOfCrossPenetrationPtr = std::shared_ptr<IWeldLandOfCrossPenetration>;
    using IWeldLandOfCrossPenetrationConstPtr = std::shared_ptr<const IWeldLandOfCrossPenetration>;
  }
}