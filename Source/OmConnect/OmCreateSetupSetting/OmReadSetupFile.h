#ifndef OM_READ_SETUP_FILE_H
#define OM_READ_SETUP_FILE_H
#include "../pch.h"


using namespace std;
using namespace std::experimental::filesystem;
using namespace Olympus::FileManagement;
using namespace Olympus::FileManagement::Storage;
using namespace Olympus::Ultrasound;
using namespace Olympus::Inspection;
using namespace Olympus::Equipment;
using namespace Olympus::PartDefinition;
using namespace Instrumentation;

std::filesystem::path getFilePath();
class OmReadSetupFile
{
public:
    static bool read_setup() {
        try
        {
            auto inputFile = getFilePath();
            auto setupFile = OpenSetupFile(inputFile);
            ReadSetup(setupFile->GetSetup());
        }
        catch (std::exception const& ex)
        {
            cout << ex.what() << endl;
            return false;
        }

        return true;
    }


private:

    static void ReadSpecimen(IScanPlanPtr scanPlan)
    {
        auto specimen = scanPlan->GetSpecimen();
        auto material = specimen->GetMaterial();
        wstring name = material->GetName();
        double longiVelocity = material->GetLongitudinalVelocity();
        double shearVelocity = material->GetShearVelocity();

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
        cout << "Specimen" << endl;
    }

    static void ReadProbe(IScanPlanPtr scanPlan)
    {
        auto inspectionMethods = scanPlan->GetInspectionMethodCollection();
        for (size_t i(0); i < inspectionMethods->GetCount(); ++i)
        {
            auto inspectionMethod = inspectionMethods->GetInspectionMethod(i);
            if (auto pulseEchoMethod = dynamic_pointer_cast<IInspectionMethodPulseEcho>(inspectionMethod))
            {
                auto probe = pulseEchoMethod->GetProbe(); // Only conventional probe.
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
                    if (auto wedge = dynamic_pointer_cast<IWedgeConventional>(probe->GetWedge())) // Only conventional wedge.
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
            else if (auto phasedArrayMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspectionMethod))
            {
                auto probe = phasedArrayMethod->GetProbe();
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
            else if (auto tofdMethod = dynamic_pointer_cast<IInspectionMethodTofd>(inspectionMethod))
            {
                //TODO
            }
            else
                throw bad_cast{};
        }
    }

    static void ReadInspectionMethod(IScanPlanPtr scanPlan)
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
                auto probe = pulseEchoMethod->GetProbe(); // Only conventional probe.
                wstring name = probe->GetName();

                auto probePosition = pulseEchoMethod->GetProbePosition();
                double x = probePosition->GetX();
                double y = probePosition->GetY();
                double angle = probePosition->GetAngle();

                auto beam = pulseEchoMethod->GetBeam();
                auto waveType = beam->GetWaveType();
            }
            else if (auto phasedArrayMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspectionMethod))
            {
                auto probe = phasedArrayMethod->GetProbe();
                wstring name = probe->GetName();

                auto probePosition = phasedArrayMethod->GetProbePosition();
                double x = probePosition->GetX();
                double y = probePosition->GetY();
                double angle = probePosition->GetAngle();

                auto beamSet = phasedArrayMethod->GetBeamSet();
                auto waveType = beamSet->GetWaveType();
                auto depthFocusing = beamSet->GetFocusing()->GetDepthFocusing();
                double distance = beamSet->GetFocusing()->GetDistance();
            }
            else if (auto tofdMethod = dynamic_pointer_cast<IInspectionMethodTofd>(inspectionMethod))
            {
                //TODO
            }
            else
                throw bad_cast{};
        }
    }

    static void ReadUltrasoundConfig(IConfigurationCollectionPtr configs)
    {
        for (size_t configIdx(0); configIdx < configs->GetCount(); ++configIdx)
        {
            auto config = configs->GetConfiguration(configIdx);
            auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
            if (paConfig)
            {
                wstring beamSetName = paConfig->GetInspectionMethod()->GetName();
                double gain = paConfig->GetGain();
                double wedgeDelay = paConfig->GetWedgeDelay();

                auto digitizingSettings = paConfig->GetDigitizingSettings();
                int compressionFactor = digitizingSettings->GetTimeSettings()->GetAscanCompressionFactor();

                auto filterSettings = digitizingSettings->GetFilterSettings();
                bool smoothingEnabled = filterSettings->IsSmoothingFilterEnabled();
                double smoothingFilter = filterSettings->GetSmoothingFilter();

                size_t beamQty = paConfig->GetBeamCount();
                for (size_t beamIdx(0); beamIdx < paConfig->GetBeamCount(); ++beamIdx)
                {
                    auto beam = paConfig->GetBeam(beamIdx);
                    double digitizingDelay = beam->GetDigitizingDelay();
                    double digitizingLength = beam->GetDigitizingLength();
                }

            }
            else
            {
                auto utConfig = dynamic_pointer_cast<IConventionalConfiguration>(config);
                if (utConfig)
                {
                    wstring beamName = utConfig->GetInspectionMethod()->GetName();
                    double gain = utConfig->GetGain();
                    double wedgeDelay = utConfig->GetWedgeDelay();
                    double digitizingDelay = utConfig->GetDigitizingDelay();
                    double digitizingLength = utConfig->GetDigitizingLength();

                    auto digitizingSettings = utConfig->GetDigitizingSettings();
                    int compressionFactor = digitizingSettings->GetTimeSettings()->GetAscanCompressionFactor();

                    auto filterSettings = digitizingSettings->GetFilterSettings();
                    bool smoothingEnabled = filterSettings->IsSmoothingFilterEnabled();
                    double smoothingFilter = filterSettings->GetSmoothingFilter();

                }
                else
                    throw bad_cast{};
            }
        }
    }

    static void ReadInspectionConfig(IInspectionConfigurationCollectionPtr inspConfigs)
    {
        for (size_t inspConfigIdx(0); inspConfigIdx < inspConfigs->GetCount(); ++inspConfigIdx)
        {
            auto inpsConfig = inspConfigs->GetConfiguration(inspConfigIdx);
            double acqRate = inpsConfig->GetRate();
            auto firingTrigger = inpsConfig->GetFiringTrigger();

            for (size_t encoderIdx(0); encoderIdx < inpsConfig->GetEncoderConfigurations()->GetCount(); ++encoderIdx)
            {
                auto encoder = inpsConfig->GetEncoderConfigurations()->GetEncoderConfiguration(encoderIdx);
                wstring name = encoder->GetName();
                bool inverted = encoder->IsInverted();
                auto encoderType = encoder->GetType();
                size_t presetValue = encoder->GetPresetValue();
                double distanceResolution = encoder->GetDistanceResolution();
                double paceResolution = encoder->GetFiringResolution();
            }

            for (size_t acqUnitidx(0); acqUnitidx < inpsConfig->GetAcquisitionUnitConfigurations()->GetCount(); ++acqUnitidx)
            {
                auto acqUnitConfig = inpsConfig->GetAcquisitionUnitConfigurations()->GetConfiguration(acqUnitidx);
                wstring deviceName = acqUnitConfig->GetName();
                auto platform = acqUnitConfig->GetPlatform();
                auto model = acqUnitConfig->GetModel();

                auto digitizer = acqUnitConfig->GetUltrasoundDigitizerConfiguration();

                double pulserVoltage{};
                auto convDigitizer = digitizer->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::Conventional);
                for (size_t pulserVoltageIdx(0); pulserVoltageIdx < convDigitizer->GetPulserVoltages()->GetCount(); ++pulserVoltageIdx)
                {
                    pulserVoltage = convDigitizer->GetPulserVoltages()->GetPulserVoltage(pulserVoltageIdx);
                }

                double convPulserVoltage = convDigitizer->GetPulserVoltage();

                auto paDigitizer = digitizer->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::PhasedArray);
                for (size_t pulserVoltageIdx(0); pulserVoltageIdx < paDigitizer->GetPulserVoltages()->GetCount(); ++pulserVoltageIdx)
                {
                    pulserVoltage = paDigitizer->GetPulserVoltages()->GetPulserVoltage(pulserVoltageIdx);
                }

                double paPulserVoltage = paDigitizer->GetPulserVoltage();

            }

            ReadUltrasoundConfig(inpsConfig->GetConfigurations());
        }
    }

    static void ReadSetup(ISetupPtr setup)
    {
        ReadSpecimen(setup->GetScanPlan());
        ReadProbe(setup->GetScanPlan());
        ReadInspectionMethod(setup->GetScanPlan());
        ReadInspectionConfig(setup->GetInspectionConfigurations());
    }

};

#endif