#include "OpenView.ScanPlan.h"
std::shared_ptr<Om_Settup_Config> OpenView::ScanPlan::omSetCof = nullptr;

namespace OpenView
{
    void ScanPlan::Create(ISetupPtr setup)
    {
        if (!omSetCof)
            omSetCof = OmSetupL::getInstance().OMS;

        Olympus::PartDefinition::IScanPlanPtr scanPlan = setup->GetScanPlan();
        auto specimen = scanPlan->CreatePlateSpecimen(180.,45.,130.);
        //Material
        auto material = specimen->GetMaterial();
        material->SetName(L"STEEL, MILD");
        material->SetShearVelocity(6570);
        material->SetLongitudinalVelocity(5890.);
        // Acquisition Unit

        auto equipfactory = scanPlan->GetEquipmentFactory();
        auto acqUnit = equipfactory->AddAcquisitionUnit(L"omniScanX3",
            IDeviceInfo::Platform::OmniScanX3, IDeviceInfo::Model::PA32x128);

        // Encoder
        //auto encoder = equipfactory->CreateEncoder(L"Scan Encoder", 10.);
        auto ioPort = acqUnit->FindInputOutputPort(OmniScanX3::PA_PORT);

        auto patch = scanPlan->GetPatches()->GetPatch(0);
        //patch->GetScanAxis()->SetEncoder(encoder);
        patch->SetScanType(ScanType::OneLineScan);

        // Methods

        AddLinearInspectionMethod(scanPlan);
    }
    void ScanPlan::AddLinearInspectionMethod(IScanPlanPtr scanPlan)
    {
        // PROBES
        auto equipfactory = scanPlan->GetEquipmentFactory();        
        auto raTransducer = equipfactory->CreateTransducerRectangularArray(L"5L64-A2", omSetCof->raProbeFre,
            omSetCof->raProbeElem, omSetCof->raProbeRows, omSetCof->raProbeWidth, omSetCof->raProbeHeight);  // 64 elements, 1 row (Linear array)
        // auto raWedge = equipfactory->CreateWedgeRectangularArray(L"SA31-N55S", 48.58, 30., 32.26, 36.1);
        // raWedge->SetMaterialVelocity(2330.);

        auto raProbe = equipfactory->CreateRectangularArray(L"Linear Array Probe", raTransducer/*, raWedge*/);
        auto acqUnit = scanPlan->GetAcquisitionUnits()->GetAcquisitionUnit(0);
        auto paPort = acqUnit->FindMultiPulserReceiverPort(OmniScanX3::PA_PORT); 
        raProbe->GetConnector()->SetConnection(paPort);

        // Phased array linear inspection method
        auto inspFactory = scanPlan->GetInspectionMethodFactory();
        auto patch = scanPlan->GetPatches()->GetPatch(0);          
        auto linearFormation = inspFactory->CreateLinearFormation(0., omSetCof->EleQuantity, omSetCof->EleFirst, omSetCof->EleLast, omSetCof->EleStep);  // Linear formation 
        auto linearMethod = inspFactory->AddPhasedArrayLinear(L"GR-2", patch, raProbe, linearFormation);

        linearMethod->GetBeamSet()->SetWaveType(omSetCof->LineaerWaveType);
        linearMethod->GetBeamSet()->GetFocusing()->SetDepthFocusing(DepthFocusing::TrueDepth);
        linearMethod->GetBeamSet()->GetFocusing()->SetDistance(omSetCof->beamFocusingDepth);

        linearMethod->GetProbePosition()->SetAngle(0);
        linearMethod->GetProbePosition()->SetX(0.);
        linearMethod->GetProbePosition()->SetY(0.);

        auto linearFormationPtr = dynamic_pointer_cast<IFormationLinear>(linearMethod->GetBeamSet()->GetFormation());


        // In linear array, no need for skew angles (sectorial scanning), so skip setting skew angles.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

}
