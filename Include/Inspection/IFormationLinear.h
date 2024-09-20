///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IFormationLinear.h.
///
/// Declares the IFormationLinear interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus { namespace Inspection {

  class IFormationLinear : public IFormation
  {
  public:
    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Destructor
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IFormationLinear() = default;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the angle.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "beamRefractedAngle"
    ///
    /// @returns The angle.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetAngle() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets an angle.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "beamRefractedAngle"
    ///
    /// @param angle The angle.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetAngle(double angle) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the step element.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "elementStep"
    ///
    /// @returns The step element.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual int GetStepElement() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a step element.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "elementStep"
    ///
    /// @param stepElement The step element.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetStepElement(int stepElement) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the first element of first law.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "probeFirstElementId"
    ///
    /// @returns The first element of first law.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual int GetFirstElementOfFirstLaw() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a first element of first law.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "probeFirstElementId"
    ///
    /// @param firstElementOfFirstLaw The first element of first law.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetFirstElementOfFirstLaw(int firstElementOfFirstLaw) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the last element of last law.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "probeLastElementId"
    ///
    /// @returns The last element of last law.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual int GetLastElementOfLastLaw() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a last element of last law.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "probeLastElementId"
    ///
    /// @param lastElementOfLastLaw The last element of last law.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetLastElementOfLastLaw(int lastElementOfLastLaw) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the step element.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "elementStep"
    ///
    /// @returns The step element.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetStepElementFloat() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a step element.
    ///
    /// NDE file domain path "dataGroups"[index] / "ultrasound" / "linearPulseEcho" / "elementStep"
    ///
    /// @param stepElement The step element.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetStepElementFloat(double stepElement) = 0;
  };

  using IFormationLinearPtr = std::shared_ptr<IFormationLinear>;
  using IFormationLinearConstPtr = std::shared_ptr<const IFormationLinear>;
}}