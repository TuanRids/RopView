///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IFocusingZone.h.
///
/// NDE file domain path: "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "tfmDescription" / "rectangularGrid"
/// 
/// Declares the IFocusingZone interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <inspection/IFocusingLimits.h>

namespace Olympus {
  namespace Inspection
  {
    class IFocusingZone
    {
    public:
      virtual ~IFocusingZone() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the index limits of the zone
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "tfmDescription" / "rectangularGrid" / "yImagingLimits"
      /// 
      /// @returns The focusing limits for the index axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFocusingLimitsPtr GetIndexLimits() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the depth limits of the zone
      ///
      /// NDE file domain path "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "tfmDescription" / "rectangularGrid" / "zImagingLimits"
      /// 
      /// @returns The focusing limits for the ultrasound axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFocusingLimitsPtr GetDepthLimits() const = 0;
    };

    using IFocusingZonePtr = std::shared_ptr<IFocusingZone>;
    using IFocusingZoneConstPtr = std::shared_ptr<const IFocusingZone>;
  }
}
