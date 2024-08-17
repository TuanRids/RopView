///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IInspectionMethodPitchCatch.h.
///
/// NDE file domain path  dataGroups[index] / ultrasound / conventionalPitchCatch
/// 
/// Declares the IInspectionMethodPitchCatch interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IProbeConventional.h>

namespace Olympus { namespace Inspection {

  class ITransformation2D;
  using ITransformation2DPtr = std::shared_ptr<ITransformation2D>;
  using ITransformation2DConstPtr = std::shared_ptr<const ITransformation2D>;

  class IInspectionMethodPitchCatch : public IInspectionMethodConventional
  {
    public:
    
    virtual ~IInspectionMethodPitchCatch() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the Pulsing probe.
    ///
    /// NDE file domain path  probes[pulserProbeId] / conventionalRound or conventionalRectangular
    /// pulserProbeId = (dataGroups[index] / ultrasound / conventionalPitchCatch / pulserProbeId)
    /// 
    /// @return The Pulsing probe.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Equipment::IProbeConventionalPtr GetProbePulsing() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a Pulsing probe.
    ///
    /// @param probeConventional The Pulsing probe.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbePulsing(Equipment::IProbeConventionalPtr probeConventional) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the Pulsing probe position.
    ///
    /// NDE file domain path  wedges[pulserProbeId] / positioning
    /// pulserProbeId = (dataGroups[index] / ultrasound / conventionalPitchCatch / pulserProbeId)
    /// 
    /// @return 
    /// The Pulsing probe position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransformation2DPtr GetProbePulsingPosition() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a Pulsing probe position.
    ///
    /// @param probePosition The Pulsing probe position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbePulsingPosition(ITransformation2DPtr probePosition) = 0;


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the Receiving probe.
    ///
    /// NDE file domain path  probes[receiverProbeId] / conventionalRound or conventionalRectangular
    /// receiverProbeId = (dataGroups[index] / ultrasound / conventionalPitchCatch / receiverProbeId)
    /// 
    /// @return The Receiving probe.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Equipment::IProbeConventionalPtr GetProbeReceiving() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a Receiving probe.
    ///
    /// @param probeConventional The Receiving probe.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbeReceiving(Equipment::IProbeConventionalPtr probeConventional) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the Receiving probe position.
    ///
    /// NDE file domain path  wedges[receiverProbeId] / positioning
    /// receiverProbeId = (dataGroups[index] / ultrasound / conventionalPitchCatch / pulserProbeId)
    /// 
    /// @return 
    /// The Receiving probe position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransformation2DPtr GetProbeReceivingPosition() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a probe position.
    ///
    /// @param probePosition Receiving probe position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbeReceivingPosition(ITransformation2DPtr probePosition) = 0;

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

  using IInspectionMethodPitchCatchPtr = std::shared_ptr<IInspectionMethodPitchCatch>;
  using IInspectionMethodPitchCatchConstPtr = std::shared_ptr<const IInspectionMethodPitchCatch>;
}}

