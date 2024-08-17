///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\IPlateGeometry.h.
///
/// NDE file domain path specimens[index] / plateGeometry 
/// 
/// Declares the IPlateGeometry interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <PartDefinition/IGeometry.h>
#include <PartDefinition/Surface.h>

namespace Olympus {
  namespace PartDefinition
  {
    /**
    * JSON format location: specimens.plateGeometry
    */
    class IPlateGeometry : public IGeometry
    {
    public:
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Length of the plate
      ///
      /// NDE file domain path specimens[index] / plateGeometry / length
      /// 
      /// @return
      /// The length is parallel to the scan axis and measured in millimeters
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetLength() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Width of the plate
      ///
      /// NDE file domain path specimens[index] / plateGeometry / width
      /// 
      /// @return
      /// The width is parallel to the index axis and measured in millimeters
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetWidth() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Thickness of the plate
      ///
      /// NDE file domain path specimens[index] / plateGeometry / thickness
      /// 
      /// @return
      /// The thickness is parallel to the ultrasound axis and measured in millimeters
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetThickness() const = 0;
      virtual IPlaneSurfaceConstPtr GetTopSurface() const = 0;
    };

    using IPlateGeometryPtr = std::shared_ptr<IPlateGeometry>;
    using IPlateGeometryConstPtr = std::shared_ptr<const IPlateGeometry>;
  }
}
