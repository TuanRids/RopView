///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection\IInspectionMethodTotalFocusing.h.
///
/// NDE file domain path  dataGroups[index] / ultrasound / fullMatrixCapture
/// 
/// Declares the IInspectionMethodTotalFocusing interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Inspection/IWaveTypeCollection.h>
#include <Inspection/IFocusingZone.h>
#include <Inspection/IInspectionMethod.h>
#include <Inspection/ITransformation2D.h>
#include <storage/EquipmentTypes.h>

namespace Olympus { 
  namespace Inspection 
  {
    enum class InspectionTechnique { PulseEcho, Tandem };

    class IInspectionMethodTotalFocusing : public IInspectionMethod
    {
    public:

      virtual ~IInspectionMethodTotalFocusing() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the probe.
      ///
      /// NDE file domain path  dataGroups[index] / ultrasound / fullMatrixCapture / probeId
      /// 
      /// @return
      /// The probe.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual Equipment::IProbePtr GetProbe() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a probe.
      ///
      /// @param probe The probe.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetProbe(Equipment::IProbeRectangularArrayPtr probe) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the probe position.
      ///
      /// NDE file domain path  wedges[probeId] / positioning
      /// probeId = (dataGroups[index] / ultrasound / fullMatrixCapture / probeId)
      /// 
      /// @return
      /// The probe position.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual ITransformation2DPtr GetProbePosition() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a probe position.
      ///
      /// @param probePosition The probe position.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetProbePosition(ITransformation2DPtr probePosition) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the inspection technique.
      /// Can be PulseEcho or Tandem.
      ///
      /// @return
      /// The the inspection technique.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual InspectionTechnique GetInspectionTechnique() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the inspection technique.
      /// Can be PulseEcho or Tandem.
      ///
      /// NDE file domain path: Not available
      /// 
      /// @param inspectionTechnique The inspection technique.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetInspectionTechnique(InspectionTechnique inspectionTechnique) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the pulsing wave types.
      /// Can be a combination of longitudinal or transversal wave type.
      ///
      /// NDE file domain path: dataGroups[index] / ultrasound / fullMatrixCapture / tfmDescription / waveSets / pulsing
      /// 
      /// @return
      /// The the pulsing wave types.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IWaveTypeCollectionPtr GetPulsingWaveTypes() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the receiving wave types.
      /// Can be a combination of longitudinal or transversal wave type.
      ///
      /// NDE file domain path: dataGroups[index] / ultrasound / fullMatrixCapture / tfmDescription / waveSets / receiving
      /// 
      /// @return
      /// The the receiving wave types.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IWaveTypeCollectionPtr GetReceivingWaveTypes() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the focusing zone.
      /// Limit for the computation zone
      ///
      /// NDE file domain path: "dataGroups"[index] / "ultrasound" / "fullMatrixCapture" / "tfmDescription" / "rectangularGrid"
      /// 
      /// @return
      /// The the focusing zone.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFocusingZonePtr GetFocusingZone() const = 0;

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

    using IInspectionMethodTotalFocusingPtr = std::shared_ptr<IInspectionMethodTotalFocusing>;
    using IInspectionMethodTotalFocusingConstPtr = std::shared_ptr<const IInspectionMethodTotalFocusing>;
  }
}
