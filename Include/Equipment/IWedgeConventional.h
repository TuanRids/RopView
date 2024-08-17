///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment\IWedgeConventional.h.
///
/// NDE file domain path wedges[index] / angleBeamWedge / mountingLocations[index]
/// 
/// Declares the IWedgeConventional interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IWedgeBlock.h>

namespace Olympus {
  namespace Equipment {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Physical wedge on which the conventional transducer in installed. This block receives a single
    /// element transducer.
    /// The information of this section is important to allow RayTracing inside the wedge.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class IWedgeConventional : public IWedgeBlock
    {
    public:
      virtual ~IWedgeConventional() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the exit point of the Conventional Wedge
      ///
      /// NDE file domain path wedges[index] / angleBeamWedge / mountingLocations[index] / primaryOffset
      /// 
      /// @return Distance in millimeter between the front of the wedge block and the beam exit point.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetExitPoint() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets an exit point for the Conventional Wedge
      ///
      /// @param exitPoint Distance in millimeter between the front of the wedge block and the beam exit point.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetExitPoint(double exitPoint) = 0;
    };

  using IWedgeConventionalPtr = std::shared_ptr<IWedgeConventional>;
  using IWedgeConventionalConstPtr = std::shared_ptr<const IWedgeConventional>;
}}