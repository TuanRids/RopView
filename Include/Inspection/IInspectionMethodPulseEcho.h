///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IInspectionMethodPulseEcho.h.
///
/// NDE file domain path  dataGroups[index] / ultrasound / conventionalPulseEcho
/// 
/// Declares the IInspectionMethodPulseEcho interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IProbeConventional.h>

namespace Olympus { namespace Inspection {

  class ITransformation2D;
  using ITransformation2DPtr = std::shared_ptr<ITransformation2D>;
  using ITransformation2DConstPtr = std::shared_ptr<const ITransformation2D>;

  class IInspectionMethodPulseEcho : public IInspectionMethodConventional
  {
  public:

    virtual ~IInspectionMethodPulseEcho() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the probe.
    ///          
    /// NDE file domain path  probes[probeId] / conventionalRound or conventionalRectangular
    /// probeId = (dataGroups[index] / ultrasound / conventionalPulseEcho / probeId)
    /// 
    /// @return
    /// The probe.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Equipment::IProbeConventionalPtr GetProbe() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a probe.
    ///
    /// @param probeConventional The probe conventional.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbe(Equipment::IProbeConventionalPtr probeConventional) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the probe position.
    ///
    /// NDE file domain path  wedges[probeId] / positioning
    /// probeId = (dataGroups[index] / ultrasound / conventionalPulseEcho / probeId)
    /// 
    /// @return The probe position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransformation2DPtr GetProbePosition() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a probe position.
    ///
    /// @param probePosition The probe position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbePosition(ITransformation2DPtr probePosition) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Get a string that is used to provide additionnal information on the law.
    /// This string is not validated and open for use
    /// example: "Coupling Check" "Root" "Volumetry"
    ///
    /// NDE file domain path: dataGroups[index] / usage
    /// 
    /// @return
    /// usage string.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual const std::wstring& GetUsage() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Get a string that is used to provide additionnal information on the law.
    /// This string is not validated and open for use
    /// example: "Coupling Check" "Root" "Volumetry"
    ///
    /// @param usage string.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetUsage(const std::wstring& name) = 0;
  };

  using IInspectionMethodPulseEchoPtr = std::shared_ptr<IInspectionMethodPulseEcho>;
  using IInspectionMethodPulseEchoConstPtr = std::shared_ptr<const IInspectionMethodPulseEcho>;
}}
