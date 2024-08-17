///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\ITubularGeometry.h.
///
/// NDE file domain path specimens[index] / pipeGeometry
/// 
/// Declares the ITubularGeometry interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <PartDefinition/IGeometry.h>
#include <PartDefinition/Surface.h>

namespace Olympus {
  namespace PartDefinition
  {

    enum class ScanPath
    {
      ScanAlong, ScanAround,
    };

    class ITubularGeometry : public IGeometry
    {
    public:
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Length of the tube
      ///
      /// NDE file domain path specimens[index] / pipeGeometry / length
      /// 
      /// @return
      /// The length is measured along the cylinder’s axis and measured in millimeters
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetLength() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Diameter of the tube
      ///
      /// NDE file domain path specimens[index] / pipeGeometry / outerRadius
      /// 
      /// @return
      /// The diameter of the tube measured in millimeters
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetDiameter() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Scan path of the inspection
      ///
      /// NDE file domain path Not in NDE
      /// 
      /// @return
      /// Indicates if the scan axis will be around the pipe or along the pipe
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual ScanPath GetScanPath() const = 0;
      virtual ICylinderSurfaceConstPtr GetOutsideSurface() const = 0;
    };

    using ITubularGeometryPtr = std::shared_ptr<ITubularGeometry>;
    using ITubularGeometryConstPtr = std::shared_ptr<const ITubularGeometry>;
  }
}
