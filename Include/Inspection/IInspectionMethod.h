///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IInspectionMethod.h.
///
/// 
/// Declares the IInspectionMethod interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>

namespace Olympus { namespace PartDefinition {

  class IPatch;
  using IPatchPtr = std::shared_ptr<IPatch>;
  using IPatchConstPtr = std::shared_ptr<const IPatch>;
}}

namespace Olympus { namespace Inspection {

  class IInspectionMethod
  {
  public:

    virtual ~IInspectionMethod() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the name.
    ///
    /// NDE file domain path:  dataGroups[index] / ultrasound / conventionalPulseEcho or 
    /// conventionalPitchCatch or tofd or sectorialPulseEcho or linearPulseEcho or compoundPulseEcho or 
    /// fullMatrixCapture
    /// 
    /// @return
    /// The name.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const std::wstring& GetName() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the patch.
    ///
    /// NDE file domain path:  dataEncodings[index] / discreteGrid / oneLineScan or rasterScan
    /// 
    /// @return The patch.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual PartDefinition::IPatchPtr GetPatch() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a patch.
    ///
    /// @param patch The patch.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetPatch(PartDefinition::IPatchPtr patch) = 0;
  };

  using IInspectionMethodPtr = std::shared_ptr<IInspectionMethod>;
  using IInspectionMethodConstPtr = std::shared_ptr<const IInspectionMethod>;
}}