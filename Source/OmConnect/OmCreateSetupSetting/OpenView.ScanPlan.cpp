#include "OpenView.ScanPlan.h"

namespace OpenView
{
    void ScanPlan::Create(ISetupPtr setup)
    {
        auto scanPlan = setup->GetScanPlan();
        auto specimen = scanPlan->CreateWeldedPipeSpecimen(1000., 300., ScanPath::ScanAround);

        // Material
        auto material = specimen->GetMaterial();
        material->SetName(L"STEEL, MILD");
        material->SetShearVelocity(3240.);
        material->SetLongitudinalVelocity(5890.);

        // Weld
        auto weld = specimen->CreateWeld(WeldBevel::V, WeldType::symmetric);
        weld->GetLandOfCrossPenetration()->SetHeight(2.);
        weld->GetHotPass()->SetAngle(30.);
        weld->GetHotPass()->SetHeight(6.5);
        weld->GetFills()->AddFill(6.5, 0.);

        // Acquisition Unit

        auto equipfactory = scanPlan->GetEquipmentFactory();
        auto acqUnit = equipfactory->AddAcquisitionUnit(L"Unit 1",
            IDeviceInfo::Platform::FocusPx, IDeviceInfo::Model::PA32x128);

        // Encoder
        auto encoder = equipfactory->CreateEncoder(L"Scan Encoder", 10.);
        auto ioPort = acqUnit->FindInputOutputPort(FocusPX::GPIO_IO_PORT);
        auto phaseA_axis1 = ioPort->FindPin(PinSignal::PhaseA_axis1);
        auto phaseB_axis1 = ioPort->FindPin(PinSignal::PhaseB_axis1);
        encoder->SetPhaseAPin(phaseA_axis1);
        encoder->SetPhaseBPin(phaseB_axis1);

        auto patch = scanPlan->GetPatches()->GetPatch(0);
        patch->GetScanAxis()->SetEncoder(encoder);
        patch->SetScanType(ScanType::OneLineScan);

        // Methods

        //AddTofdInspectionMethod(scanPlan);
        //AddPulseEchoInspectionMethod(scanPlan);
        AddSectorialInspectionMethod(scanPlan);
        // AddLinearInspectionMethod(scanPlan);
    }
    void ScanPlan::AddLinearInspectionMethod(IScanPlanPtr scanPlan)
    {
        auto equipfactory = scanPlan->GetEquipmentFactory();

        // Rectangular array probe (Linear Array Probe)
        auto raTransducer = equipfactory->CreateTransducerRectangularArray(L"5L32-A31", 5., 32, 1, 0.6, 10.);  // 32 elements, 1 row (Linear array)
        // auto raWedge = equipfactory->CreateWedgeRectangularArray(L"SA31-N55S", 48.58, 30., 32.26, 36.1);
        // raWedge->SetMaterialVelocity(2330.);

        auto raProbe = equipfactory->CreateRectangularArray(L"Linear Array Probe", raTransducer/*, raWedge*/);
        auto acqUnit = scanPlan->GetAcquisitionUnits()->GetAcquisitionUnit(0);
        auto paPort = acqUnit->FindMultiPulserReceiverPort(OmniScanX3::PA_PORT);  // Port for phased array
        raProbe->GetConnector()->SetConnection(paPort);

        // Phased array linear inspection method
        auto inspFactory = scanPlan->GetInspectionMethodFactory();
        auto patch = scanPlan->GetPatches()->GetPatch(0);

        // Create linear formation
        auto linearFormation = inspFactory->CreateLinearFormation(1., 32, 1, 64, 1);  // Linear formation with 32 elements
        auto linearMethod = inspFactory->AddPhasedArrayLinear(L"GR-2", patch, raProbe, linearFormation);

        linearMethod->GetBeamSet()->SetWaveType(WaveType::Transversal);
        linearMethod->GetBeamSet()->GetFocusing()->SetDepthFocusing(DepthFocusing::TrueDepth);
        linearMethod->GetBeamSet()->GetFocusing()->SetDistance(50.);

        linearMethod->GetProbePosition()->SetAngle(90.);
        linearMethod->GetProbePosition()->SetX(0.);
        linearMethod->GetProbePosition()->SetY(-20.);

        auto linearFormationPtr = dynamic_pointer_cast<IFormationLinear>(linearMethod->GetBeamSet()->GetFormation());

        // In linear array, no need for skew angles (sectorial scanning), so skip setting skew angles.
    }

    void ScanPlan::AddPulseEchoInspectionMethod(IScanPlanPtr scanPlan)
    {
        auto equipfactory = scanPlan->GetEquipmentFactory();

        // Conventional probe
        auto convTransducer = equipfactory->CreateTransducerConventionalRound(L"C551-SM", 5., 9.5);
        auto convWedge = equipfactory->CreateWedgeConventional(L"SPE2-60S-IHC", 28.69, 31.75, 17.39, 36.1);
        convWedge->SetMaterialVelocity(2330.);

        auto convProbe = equipfactory->CreateConventionalRound(L"Conventional Probe", convTransducer, convWedge);
        auto acqUnit = scanPlan->GetAcquisitionUnits()->GetAcquisitionUnit(0);
        auto convPort = acqUnit->FindSinglePulserReceiverPort(FocusPX::UT_P1R1_PORT);
        convProbe->GetConnector()->SetConnection(convPort);

        // Conventional inspection method
        auto inspFactory = scanPlan->GetInspectionMethodFactory();
        auto patch = scanPlan->GetPatches()->GetPatch(0);
        auto methodConv = inspFactory->AddConventionalPulseEcho(L"GR-1", patch, convProbe);
        methodConv->GetBeam()->SetWaveType(WaveType::Transversal);

        methodConv->GetProbePosition()->SetAngle(270.);
        methodConv->GetProbePosition()->SetX(0.);
        methodConv->GetProbePosition()->SetY(10.);
    }

    void ScanPlan::AddSectorialInspectionMethod(IScanPlanPtr scanPlan)
    {
        auto equipfactory = scanPlan->GetEquipmentFactory();

        // Rectangular array probe
        auto raTransducer = equipfactory->CreateTransducerRectangularArray(L"5L32-A31", 5., 32, 1, 0.6, 10.);
        auto raWedge = equipfactory->CreateWedgeRectangularArray(L"SA31-N55S", 48.58, 30., 32.26, 36.1);
        raWedge->SetMaterialVelocity(2330.);

        auto raProbe = equipfactory->CreateRectangularArray(L"Phased Array Probe", raTransducer, raWedge);
        auto acqUnit = scanPlan->GetAcquisitionUnits()->GetAcquisitionUnit(0);
        auto paPort = acqUnit->FindMultiPulserReceiverPort(OmniScanX3::PA_PORT);
        raProbe->GetConnector()->SetConnection(paPort);

        // Phased array sectorial inspection method
        auto inspFactory = scanPlan->GetInspectionMethodFactory();
        auto patch = scanPlan->GetPatches()->GetPatch(0);
        auto sectoFormation = inspFactory->CreateSectorialFormation(45., 49., 1., 1, 16);
        auto sectoMethod = inspFactory->AddPhasedArraySectorial(L"GR-2", patch, raProbe, sectoFormation);

        sectoMethod->GetBeamSet()->SetWaveType(WaveType::Transversal);
        sectoMethod->GetBeamSet()->GetFocusing()->SetDepthFocusing(DepthFocusing::TrueDepth);
        sectoMethod->GetBeamSet()->GetFocusing()->SetDistance(50.);

        sectoMethod->GetProbePosition()->SetAngle(90.);
        sectoMethod->GetProbePosition()->SetX(0.);
        sectoMethod->GetProbePosition()->SetY(-20.);

        auto sectorialFormation = dynamic_pointer_cast<IFormationSectorial>
            (sectoMethod->GetBeamSet()->GetFormation());

        sectorialFormation->GetSkewAngle()->SetStart(45.);
        sectorialFormation->GetSkewAngle()->SetStop(45.);
        sectorialFormation->GetSkewAngle()->SetStep(1.);
    }/*GetPulsingSettings()->GetAscanAveragingFactor*/

    void ScanPlan::AddTofdInspectionMethod(IScanPlanPtr scanPlan)
    {
        auto equipfactory = scanPlan->GetEquipmentFactory();
        auto acqUnit = scanPlan->GetAcquisitionUnits()->GetAcquisitionUnit(0);
        auto multiPrPort = acqUnit->FindMultiPulserReceiverPort(FocusPX::PA_PORT);
        auto singlePrPorts = multiPrPort->PutSingleElementPortSplitter(4);

        // Pulsing probe
        auto convTransducer = equipfactory->CreateTransducerConventionalRound(L"C551-SM", 5., 9.5);
        auto convWedge = equipfactory->CreateWedgeConventional(L"SPE2-60S-IHC", 28.69, 31.75, 17.39, 36.1);
        convWedge->SetMaterialVelocity(2330.);

        auto pulsingProbe = equipfactory->CreateConventionalRound(L"Pulsing Probe", convTransducer, convWedge);
        auto pulsingPort = singlePrPorts->GetPort(0);
        pulsingPort->SetConnectedPin(127);
        pulsingProbe->GetConnector()->SetConnection(pulsingPort);

        // Receiving probe
        auto receivingProbe = equipfactory->CreateConventionalRound(L"Receiving Probe", convTransducer, convWedge);
        auto receivingPort = singlePrPorts->GetPort(1);
        receivingPort->SetConnectedPin(128);
        receivingProbe->GetConnector()->SetConnection(receivingPort);

        // Tofd inspection method
        auto inspFactory = scanPlan->GetInspectionMethodFactory();
        auto patch = scanPlan->GetPatches()->GetPatch(0);
        auto methodTofd = inspFactory->AddConventionalTOFD(L"GR-1", patch, pulsingProbe, receivingProbe);
        methodTofd->GetBeam()->SetWaveType(WaveType::Transversal);

        auto centerPosition = methodTofd->GetCenterPosition();
        centerPosition->SetAngle(90.0);
        methodTofd->SetSeparation(300.);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    void ScanPlan::Read(IScanPlanPtr scanPlan)
    {
        ReadSpecimen(scanPlan);
        ReadInspectionMethod(scanPlan);
    }

    void ScanPlan::ReadSpecimen(IScanPlanPtr scanPlan)
    {
        auto specimen = scanPlan->GetSpecimen();

        auto material = specimen->GetMaterial();
        wstring name = material->GetName();
        double longiVelocity = material->GetLongitudinalVelocity();
        double shearVelocity = material->GetShearVelocity();

        if (auto weldedSpecimen = dynamic_pointer_cast<IWeldedSpecimen>(specimen))
        {
            auto weld = weldedSpecimen->GetWeld();
            WeldBevel bevelShape = weld->GetBevelShape();
            WeldType weldType = weld->GetWeldType();
            double hazWidth = weld->GetHeatAffectedZoneWidth();
            auto lcp = weld->GetLandOfCrossPenetration();
            double thickness = weld->GetThickness();
            auto hotPass = weld->GetHotPass();
            auto fills = weld->GetFills();
            auto root = weld->GetRoot();
            auto cap = weld->GetCap();
        }

        auto patchs = scanPlan->GetPatches();

        IPlaneSurfaceConstPtr topSurface;
        ICylinderSurfaceConstPtr outsideSurface;
        ICylinderSurfaceConstPtr insideSurface;

        auto geometry = specimen->GetGeometry();
        if (auto plateGeo = dynamic_pointer_cast<IPlateGeometry>(geometry))
        {
            double width = plateGeo->GetWidth();
            double length = plateGeo->GetLength();
            double thickness = plateGeo->GetThickness();
            topSurface = plateGeo->GetTopSurface();

        }
        else if (auto pipeGeo = dynamic_pointer_cast<IPipeGeometry>(geometry))
        {
            double diameter = pipeGeo->GetDiameter();
            double length = pipeGeo->GetLength();
            double thickness = pipeGeo->GetThickness();
            ScanPath scanPath = pipeGeo->GetScanPath();

            outsideSurface = pipeGeo->GetOutsideSurface();
            insideSurface = pipeGeo->GetInsideSurface();

        }
        else if (auto barGeo = dynamic_pointer_cast<IBarGeometry>(geometry))
        {
            double diameter = barGeo->GetDiameter();
            double length = barGeo->GetLength();
            ScanPath scanPath = pipeGeo->GetScanPath();
            outsideSurface = barGeo->GetOutsideSurface();
        }
        else
            throw bad_cast{};
    }

    void ScanPlan::ReadInspectionMethod(IScanPlanPtr scanPlan)
    {
        IPlaneSurfaceConstPtr topSurface;
        ICylinderSurfaceConstPtr outsideSurface;
        ICylinderSurfaceConstPtr insideSurface;

        auto geometry = scanPlan->GetSpecimen()->GetGeometry();
        if (auto plateGeo = dynamic_pointer_cast<IPlateGeometry>(geometry))
        {
            topSurface = plateGeo->GetTopSurface();
        }
        else if (auto pipeGeo = dynamic_pointer_cast<IPipeGeometry>(geometry))
        {
            outsideSurface = pipeGeo->GetOutsideSurface();
            insideSurface = pipeGeo->GetInsideSurface();
        }
        else if (auto barGeo = dynamic_pointer_cast<IBarGeometry>(geometry))
        {
            outsideSurface = barGeo->GetOutsideSurface();
        }
        else
            throw bad_cast{};

        auto inspectionMethods = scanPlan->GetInspectionMethodCollection();
        for (size_t i(0); i < inspectionMethods->GetCount(); ++i)
        {
            auto inspectionMethod = inspectionMethods->GetInspectionMethod(i);
            wstring name = inspectionMethod->GetName();

            if (auto pulseEchoMethod = dynamic_pointer_cast<IInspectionMethodPulseEcho>(inspectionMethod))
            {
                auto probe = pulseEchoMethod->GetProbe();
                ReadProbeConventional(probe);

                auto probePosition = pulseEchoMethod->GetProbePosition();
                double x = probePosition->GetX();
                double y = probePosition->GetY();
                double angle = probePosition->GetAngle();

                auto beam = pulseEchoMethod->GetBeam();
                auto waveType = beam->GetWaveType();
            }
            else if (auto pitchCatchMethod = dynamic_pointer_cast<IInspectionMethodPitchCatch>(inspectionMethod))
            {
                auto probePulsing = pitchCatchMethod->GetProbePulsing();
                ReadProbeConventional(probePulsing);

                auto probePulsingPosition = pitchCatchMethod->GetProbePulsingPosition();
                double xP = probePulsingPosition->GetX();
                double yP = probePulsingPosition->GetY();
                double angleP = probePulsingPosition->GetAngle();

                auto probeReceiving = pitchCatchMethod->GetProbeReceiving();
                ReadProbeConventional(probeReceiving);

                auto probeReceivingPosition = pitchCatchMethod->GetProbeReceivingPosition();
                double xR = probeReceivingPosition->GetX();
                double yR = probeReceivingPosition->GetY();
                double angleR = probeReceivingPosition->GetAngle();

                auto beam = pitchCatchMethod->GetBeam();
                auto waveType = beam->GetWaveType();
            }
            else if (auto phasedArrayMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspectionMethod))
            {
                auto probe = phasedArrayMethod->GetProbe();
                ReadProbeRectangularArray(probe);

                auto probePosition = phasedArrayMethod->GetProbePosition();
                double x = probePosition->GetX();
                double y = probePosition->GetY();
                double angle = probePosition->GetAngle();

                auto beamSet = phasedArrayMethod->GetBeamSet();
                auto waveType = beamSet->GetWaveType();
                auto depthFocusing = beamSet->GetFocusing()->GetDepthFocusing();
                double distance = beamSet->GetFocusing()->GetDistance();

                auto sectorialFormation = dynamic_pointer_cast<IFormationSectorial>(beamSet->GetFormation());
                if (sectorialFormation && sectorialFormation->GetSkewAngle())
                {
                    auto skewStart = sectorialFormation->GetSkewAngle()->GetStart();
                    auto skewStop = sectorialFormation->GetSkewAngle()->GetStop();
                    auto skewStep = sectorialFormation->GetSkewAngle()->GetStep();
                }
            }
            else if (auto tofdMethod = dynamic_pointer_cast<IInspectionMethodTofd>(inspectionMethod))
            {
                auto pulsingProbe = tofdMethod->GetProbePulsing();
                auto connection = pulsingProbe->GetConnector()->GetConnection();
                wstring acqUnitName = connection->GetAcquisitionUnit()->GetName();
                wstring portName = connection->GetName();
                size_t pinId = connection->GetConnectedPin();

                auto receivingProbe = tofdMethod->GetProbeReceiving();
                connection = receivingProbe->GetConnector()->GetConnection();
                acqUnitName = connection->GetAcquisitionUnit()->GetName();
                portName = connection->GetName();
                pinId = connection->GetConnectedPin();

                double separation = tofdMethod->GetSeparation();
            }
            else if (auto tfmMethod = dynamic_pointer_cast<IInspectionMethodTotalFocusing>(inspectionMethod))
            {
                auto probe = tfmMethod->GetProbe();
            }
            else {
                throw bad_cast{};
            }
        }
    }

    void ScanPlan::ReadProbeConventional(IProbeConventionalPtr probe)
    {
        wstring name = probe->GetName();

        double primaryOffset = probe->GetElementOffset()->GetPrimary();
        double secondaryOffset = probe->GetElementOffset()->GetSecondary();
        double tertiaryOffset = probe->GetElementOffset()->GetTertiary();

        auto transducer = probe->GetTransducer();
        auto model = transducer->GetModel();
        double frequency = transducer->GetFrequency();

        if (auto roundTransducer = dynamic_pointer_cast<ITransducerConventionalRound>(transducer))
        {
            double diameter = roundTransducer->GetDiameter();
        }
        else if (auto recTransducer = dynamic_pointer_cast<ITransducerConventionalRectangular>(transducer))
        {
            double width = recTransducer->GetWidth();
            double length = recTransducer->GetLength();
        }
        else
            throw bad_cast{};

        if (probe->HasWedge())
        {
            if (auto wedge = dynamic_pointer_cast<IWedgeConventional>(probe->GetWedge()))
            {
                wstring model = wedge->GetModel();
                double materialVelo = wedge->GetMaterialVelocity();
                double length = wedge->GetLength();
                double width = wedge->GetWidth();
                double height = wedge->GetHeight();
                double angle = wedge->GetAngle();
                double exitPoint = wedge->GetExitPoint();
            }
        }
    }

    void ScanPlan::ReadProbeRectangularArray(IProbePtr probe)
    {
        wstring name = probe->GetName();

        double primaryOffset = probe->GetElementOffset()->GetPrimary();
        double secondaryOffset = probe->GetElementOffset()->GetSecondary();
        double tertiaryOffset = probe->GetElementOffset()->GetTertiary();

        double primaryReverseOffset = probe->GetReversedElementOffset()->GetPrimary();
        double secondaryReverseOffset = probe->GetReversedElementOffset()->GetSecondary();
        double tertiaryReverseOffset = probe->GetReversedElementOffset()->GetTertiary();

        if (auto probeRecArray = dynamic_pointer_cast<IProbeRectangularArray>(probe))
        {
            auto transducer = probeRecArray->GetTransducer();
            auto model = transducer->GetModel();
            double frequency = transducer->GetFrequency();

            if (auto transducerRecArray = dynamic_pointer_cast<ITransducerRectangularArray>(transducer))
            {
                int elemntQty = transducerRecArray->GetNumberElement();
                int elemntQtyPrimary = transducerRecArray->GetNumberElementOnPrimaryAxis();
                int elemntQtySecondary = transducerRecArray->GetNumberElementOnSecondaryAxis();
                double elementGapPrimary = transducerRecArray->GetElementGapOnPrimaryAxis();
                double elementGapSecondary = transducerRecArray->GetElementGapOnSecondaryAxis();
                double elementWidth = transducerRecArray->GetElementWidth();
                double elementLength = transducerRecArray->GetElementLength();

                auto elements = transducerRecArray->GetElements();
                for (size_t i(0); i < elements->GetCount(); ++i)
                {
                    auto element = elements->GetElement(i);
                    bool enabled = element->IsEnabled();
                    int id = element->GetId();
                    int primaryIndex = element->GetPrimaryIndex();
                    int secondaryIndex = element->GetSecondaryIndex();
                }
            }
            else
                throw bad_cast{};

            if (probe->HasWedge())
            {
                if (auto wedge = dynamic_pointer_cast<IWedgeRectangularArray>(probe->GetWedge()))
                {
                    wstring model = wedge->GetModel();
                    double materialVelo = wedge->GetMaterialVelocity();
                    double length = wedge->GetLength();
                    double width = wedge->GetWidth();
                    double height = wedge->GetHeight();
                    double angle = wedge->GetAngle();
                    double roof = wedge->GetRoof();
                    bool supported = wedge->IsOrientationSupported(MountingOrientation::Normal);
                    supported = wedge->IsOrientationSupported(MountingOrientation::Lateral);
                    supported = wedge->IsOrientationSupported(MountingOrientation::ReverseNormal);
                    supported = wedge->IsOrientationSupported(MountingOrientation::ReverseLateral);
                }
                else
                    throw bad_cast{};
            }
        }
        else if (auto probeDuaRecArray = dynamic_pointer_cast<IProbeDualRectangularArray>(probe))
        {
            //TODO
        }
    }
}
