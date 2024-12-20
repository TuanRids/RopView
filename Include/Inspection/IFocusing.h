///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IFocusing.h.
///
/// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho" or "linearPulseEcho" or "compoundPulseEcho" / "focusing"
/// 
/// Declares the IFocusing interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus { namespace Inspection {

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// @enum  DepthFocusing
  ///
  /// Values that represent the focusing (or focal) depth type.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  enum class DepthFocusing
  {
    TrueDepth,
    HalfPath,
  };

  class IFocusing;

  using IFocusingPtr = std::shared_ptr<IFocusing>;
  using IFocusingConstPtr = std::shared_ptr<const IFocusing>;

  class IFocusing
  {
  public:

    virtual ~IFocusing() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the distance in millimeters along the acoustic axis for the maximum amplitude response.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho" or "linearPulseEcho" or "compoundPulseEcho" / "focusing" / "distance" 
    /// 
    /// @return
    /// The distance in millimeters.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetDistance() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a distance in millimeters along the acoustic axis for the maximum amplitude response.
    ///
    /// @param distance The distance.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetDistance(double distance) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the type of depth focusing.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "sectorialPulseEcho" or "linearPulseEcho" or "compoundPulseEcho" / "focusing" / "mode" 
    /// 
    /// @return
    /// The depth focusing.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual DepthFocusing GetDepthFocusing() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets the type of depth focusing.
    ///
    /// @param depthFocusing The depth focusing.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetDepthFocusing(DepthFocusing depthFocusing) = 0;
  };
}}
