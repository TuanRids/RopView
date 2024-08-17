///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\IWeldCap.h.
///
/// NDE file domain path specimens[index] / weldGeometry / upperCap or lowerCap 
/// 
/// Declares the IWeldCap interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus {
  namespace PartDefinition
  {
    class IWeldCap
    {
    public:
      virtual ~IWeldCap() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the height of the cap
      ///
      /// NDE file domain path specimens[index] / weldGeometry / upperCap or lowerCap / height 
      /// 
      /// @return
      /// The height of the cap in millimeters.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetHeight() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the height of the cap
      ///
      /// @param height
      /// The height of the cap in millimeters.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetHeight(double height) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the width of the cap
      ///
      /// NDE file domain path specimens[index] / weldGeometry / upperCap or lowerCap / width
      /// 
      /// @return
      /// The width of the cap in millimeters.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetWidth() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Set the width of the cap
      ///
      /// @param width
      /// The width of the cap in millimeters.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetWidth(double width) = 0;
    };

    using IWeldCapPtr = std::shared_ptr<IWeldCap>;
    using IWeldCapConstPtr = std::shared_ptr<const IWeldCap>;
  }
}
