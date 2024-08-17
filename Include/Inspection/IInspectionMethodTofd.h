///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IInspectionMethodTofd.h.
///
/// NDE file domain path  dataGroups[index] / ultrasound / tofd
/// 
/// Declares the IInspectionMethodTofd interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IProbeConventional.h>

namespace Olympus { namespace Inspection {

  class ITransformation2D;
  using ITransformation2DPtr = std::shared_ptr<ITransformation2D>;
  using ITransformation2DConstPtr = std::shared_ptr<const ITransformation2D>;

  class IInspectionMethodTofd : public IInspectionMethodConventional
  {
  public:

    virtual ~IInspectionMethodTofd() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the separation of the TOFD probe pair, corresponding to the distance between the exit point
    /// of each sensor. Unit is in millimeters.
    ///
    /// NDE file domain path:  Use both probe position to calculate wedges[pulserProbeId or receiverProbeId] / positioning
    /// 
    /// @return 
    /// The separation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetSeparation() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets the separation of the TOFD probe pair, corresponding to the distance between the exit point
    /// of each sensor. Unit is in millimeters.
    ///
    /// @param separation The separation between the two probes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetSeparation(double separation) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the Pulsing probe.
    ///
    /// NDE file domain path  probes[pulserProbeId] / conventionalRound or conventionalRectangular
    /// pulserProbeId = (dataGroups[index] / ultrasound / tofd / pulserProbeId)
    /// 
    /// @return The Pulsing probe.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Equipment::IProbeConventionalPtr GetProbePulsing() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a Pulsing probe.
    ///
    /// @param probeConventional The Pulsing probe conventional.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetProbePulsing(Equipment::IProbeConventionalPtr probeConventional) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the Receiving probe.
    ///
    /// NDE file domain path  probes[receiverProbeId] / conventionalRound or conventionalRectangular
    /// receiverProbeId = (dataGroups[index] / ultrasound / tofd / receiverProbeId)
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
    /// Gets the 2 probes assembly middle position.
    ///
    /// NDE file domain path:  Use both probe position to calculate wedges[pulserProbeId or receiverProbeId] / positioning
    /// 
    /// @return 
    /// The 2 probes assembly middle position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransformation2DPtr GetCenterPosition() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a probe position.
    ///
    /// @param probePosition The 2 probes assembly middle position.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetCenterPosition(ITransformation2DPtr probePosition) = 0;

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

  using IInspectionMethodTofdPtr = std::shared_ptr<IInspectionMethodTofd>;
  using IInspectionMethodTofdConstPtr = std::shared_ptr<const IInspectionMethodTofd>;
}}

