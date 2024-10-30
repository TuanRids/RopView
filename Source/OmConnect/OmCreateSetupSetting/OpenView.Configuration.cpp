#include "OpenView.Configuration.h"
#include "SystemConfig/ConfigLocator.h"
static auto omSetup = OmSetupL::getInstance().OmSetupConf;
namespace OpenView
{
    void Configuration::Create(ISetupPtr setup)
    {
        auto sancPlan = setup->GetScanPlan();
        auto inspConfigs = setup->GetInspectionConfigurations();
        auto inspConfig = inspConfigs->Add(FiringTrigger::Encoder, 1000.);

        auto encoder = sancPlan->GetPatches()->GetPatch(0)->GetScanAxis()->GetEncoder();
        auto encoderConfig = inspConfig->GetEncoderConfigurations()->Add(encoder);
        encoderConfig->SetType(EncoderType::Quadrature);
        encoderConfig->SetFiringResolution(10.);
        encoderConfig->SetDistanceResolution(1.);

        auto acqUnits = sancPlan->GetAcquisitionUnits();
        for (size_t acqUnitIdx(0); acqUnitIdx < acqUnits->GetCount(); acqUnitIdx++)
        {
            auto acqUnit = acqUnits->GetAcquisitionUnit(acqUnitIdx);
            auto acqUnitConfig = inspConfig->GetAcquisitionUnitConfigurations()->Add(acqUnit);
            acqUnitConfig->SetSerialNumber(omSetup->acquisition_deviceSerialNumber);

            auto digitizerConfig = acqUnitConfig->GetUltrasoundDigitizerConfiguration();

            auto convDigitizerConfig = digitizerConfig->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::Conventional);
            double convVoltage = convDigitizerConfig->GetPulserVoltages()->GetPulserVoltage(2);
            convDigitizerConfig->SetPulserVoltage(convVoltage);

            auto paDigitizerConfig = digitizerConfig->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::PhasedArray);
            double paVoltage = paDigitizerConfig->GetPulserVoltages()->GetPulserVoltage(4);
            paDigitizerConfig->SetPulserVoltage(paVoltage);

        }

        auto inspMethods = sancPlan->GetInspectionMethodCollection();
        for (size_t methodIdx(0); methodIdx < inspMethods->GetCount(); ++methodIdx)
        {
            auto inspMethod = inspMethods->GetInspectionMethod(methodIdx);
            if (auto convMethod = dynamic_pointer_cast<IInspectionMethodConventional>(inspMethod))
            {
                auto config = inspConfig->GetConfigurations()->AddConventional(convMethod);
                ConfigureConventional(config);
            }
            else if (auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspMethod))
            {
                auto config = inspConfig->GetConfigurations()->AddPhasedArray(paMethod);
                ConfigurePhasedArray(config);
            }
            else
                throw bad_cast{};
        }
    }

    void Configuration::ConfigureConventional(IConventionalConfigurationPtr config)
    {
        std::string applicationJsonStr = "{\"pipeWizard\": {\"Zone\": \"Volumetric\",\"Type\": \"ABC\",\"Stream\": \"Upstream\",\"GateAStart\": -3.0,\"GateALength\": 6.0,\"GateBStart\": -3.0,\"GateBLength\": 6.0,\"GateAThreshold\": 20.0,\"GateBThreshold\": 20.0,\"GateOffset\": 0.0,\"CorrectionFactor\": 0.0,\"ReflectorSize\": 1.0}}";
        config->SetApplicationSettings(applicationJsonStr);

        config->SetGain(23.4);
        config->SetVelocity(3245.);
        config->SetWedgeDelay(9000.);
        config->SetExitPoint(13.9);
        config->SetReferenceAmplitude(80.);
        config->SetDigitizingDelay(1000.);
        config->SetDigitizingLength(40960.);

        auto pulsingSettings = config->GetPulsingSettings();
        pulsingSettings->SetPulseWidth(100.);

        auto digitizingSettings = config->GetDigitizingSettings();
        digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);
        digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(IAmplitudeSettings::RectificationType::None);

        auto gateConfig = config->GetGateConfigurations();
        auto gateA = gateConfig->Add(GATE_A);
        gateA->SetDelay(10100.);
        gateA->SetLength(1000.);
        gateA->SetThreshold(30);
        gateA->ReserveCscanBuffer(true);
        gateA->SetCscanSamplingResolution(10.);
        gateA->SetPlacement(GatePlacement::TrueDepth);

        auto tcg = config->GetTcg();
        tcg->SetTcgType(ITcg::Type::Digital);
        auto tcgPoints = tcg->GetTcgPointCollection();
        tcgPoints->AddTcgPointEx(11000., 15.);
        tcgPoints->AddTcgPointEx(12000., 12.5);
        tcgPoints->AddTcgPointEx(14000., 5.);
    }

    void Configuration::ConfigurePhasedArray(IPhasedArrayConfigurationPtr config)
    {
        std::string applicationJsonStr = "{\"pipeWizard\": {\"Zone\": \"Volumetric\",\"Type\": \"ABC\",\"Stream\": \"Upstream\",\"GateAStart\": -3.0,\"GateALength\": 6.0,\"GateBStart\": -3.0,\"GateBLength\": 6.0,\"GateAThreshold\": 20.0,\"GateBThreshold\": 20.0,\"GateOffset\": 0.0,\"CorrectionFactor\": 0.0,\"ReflectorSize\": 1.0}}";
        config->SetApplicationSettings(applicationJsonStr);

        config->SetGain(omSetup->phasing_gain);
        config->SetVelocity(omSetup->phasing_velocity);
        config->SetReferenceAmplitude(omSetup->phasing_referenceAmplitude);

        auto pulsingSettings = config->GetPulsingSettings();
        pulsingSettings->SetPulseWidth(omSetup->phasing_pulseWidth);

        auto digitizingSettings = config->GetDigitizingSettings();
        digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetup->phasing_ascanDataSize);
        digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetup->phasing_rectification);

        for (size_t beamIdx(0); beamIdx < omSetup->PA_BeamQty; ++beamIdx)
        {
            auto beam = config->AddBeam();

            beam->SetBeamDelay(omSetup->PA_beamDelay[beamIdx] );
            beam->SetGainOffset(2.2);
            beam->SetDigitizingDelay(omSetup->PA_digitizingDelay[beamIdx]);
            beam->SetDigitizingLength(163840.);
            beam->SetExitPointPrimary(omSetup->PA_exitPoint[beamIdx] );
            beam->SetRefractedAnglePrimary(omSetup->PA_BeamAngleStart+ beamIdx);
            beam->SetSkewAngle(33);

            auto beamFormation = beam->CreateBeamFormation(omSetup->PA_ElementQty, omSetup->PA_ElementQty);
            auto pulserDelays = beamFormation->GetPulserDelayCollection();
            auto receiverDelays = beamFormation->GetReceiverDelayCollection();
            for (size_t elementIdx(0); elementIdx < omSetup->PA_ElementQty; ++elementIdx)
            {
                pulserDelays->GetElementDelay(elementIdx)->SetElementId(omSetup->PA_FirstElement + elementIdx);
                pulserDelays->GetElementDelay(elementIdx)->SetDelay(omSetup->PA_elementDelay[elementIdx]);

                receiverDelays->GetElementDelay(elementIdx)->SetElementId(omSetup->PA_FirstElement + elementIdx);
                receiverDelays->GetElementDelay(elementIdx)->SetDelay(omSetup->PA_elementDelay[elementIdx]);
            }

            auto gateConfig = beam->GetGateConfigurations();

            auto gateI = gateConfig->Add(GATE_I);
            gateI->SetDelay(omSetup->gate_gateIDelay);
            gateI->SetLength(omSetup->gate_gateILength);
            gateI->SetThreshold(omSetup->gate_gateIThreshold);
            gateI->SetCscanSamplingResolution(omSetup->gate_gateASamplingResolution);

            auto gateA = gateConfig->Add(GATE_A);
            gateA->SetDelay(omSetup->gate_gateADelay);
            gateA->SetLength(omSetup->gate_gateALength);
            gateA->SetThreshold(omSetup->gate_gateAThreshold);
            gateA->ReserveCscanBuffer(omSetup->gate_gateAReserveBuffer);
            gateA->SetCscanSamplingResolution(omSetup->gate_gateASamplingResolution);
            gateA->SetPlacement(omSetup->gate_gateAPlacement);

            auto gateB = gateConfig->Add(GATE_B);
            gateB->SetDelay(omSetup->gate_gateBDelay);
            gateB->SetLength(omSetup->gate_gateBLength);
            gateB->SetThreshold(omSetup->gate_gateBThreshold);
            gateB->ReserveCscanBuffer(omSetup->gate_gateBReserveBuffer);
            gateB->SetCscanSamplingResolution(omSetup->gate_gateBSamplingResolution);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    void Configuration::Read(ISetupPtr setup)
    {
        ReadInspectionConfigs(setup->GetInspectionConfigurations());
        ReadConfigurations(setup->GetInspectionConfigurations());
    }

    void Configuration::ReadInspectionConfigs(IInspectionConfigurationCollectionConstPtr inspConfigs)
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
                    pulserVoltage = convDigitizer->GetPulserVoltages()->GetPulserVoltage(pulserVoltageIdx);

                double convPulserVoltage = convDigitizer->GetPulserVoltage();

                auto paDigitizer = digitizer->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::PhasedArray);
                for (size_t pulserVoltageIdx(0); pulserVoltageIdx < paDigitizer->GetPulserVoltages()->GetCount(); ++pulserVoltageIdx)
                    pulserVoltage = paDigitizer->GetPulserVoltages()->GetPulserVoltage(pulserVoltageIdx);

                double paPulserVoltage = paDigitizer->GetPulserVoltage();
            }
        }
    }

    void Configuration::ReadConfigurations(IInspectionConfigurationCollectionConstPtr inspConfigs)
    {
        for (size_t inspConfigIdx(0); inspConfigIdx < inspConfigs->GetCount(); ++inspConfigIdx)
        {
            auto inpsConfig = inspConfigs->GetConfiguration(inspConfigIdx);
            auto configs = inpsConfig->GetConfigurations();

            for (size_t configIdx(0); configIdx < configs->GetCount(); ++configIdx)
            {
                auto config = configs->GetConfiguration(configIdx);
                if (auto convConfig = dynamic_pointer_cast<const IConventionalConfiguration>(config))
                {
                    auto jsonString = convConfig->GetApplicationSettings("MyApplication");

                    IInspectionMethodPitchCatchPtr paMethodPitchCatch = std::dynamic_pointer_cast<IInspectionMethodPitchCatch>(convConfig->GetInspectionMethod());
                    IInspectionMethodPulseEchoPtr paMethodPulseEcho = std::dynamic_pointer_cast<IInspectionMethodPulseEcho>(convConfig->GetInspectionMethod());
                    IInspectionMethodTofdPtr paMethodTofd = std::dynamic_pointer_cast<IInspectionMethodTofd>(convConfig->GetInspectionMethod());
                    wstring usage = L"";
                    if (paMethodPitchCatch != nullptr)
                    {
                        usage = paMethodPitchCatch->GetUsage();
                    }
                    else if (paMethodPulseEcho != nullptr)
                    {
                        usage = paMethodPulseEcho->GetUsage();
                    }
                    else if (paMethodTofd != nullptr)
                    {
                        usage = paMethodTofd->GetUsage();
                    }

                    wstring beamName = convConfig->GetInspectionMethod()->GetName();

                    double gain = convConfig->GetGain();
                    double analysisGain = convConfig->GetAnalysisGain();

                    double refGain{};
                    if (convConfig->HasReferenceGain())
                        refGain = convConfig->GetReferenceGain();

                    double wedgeDelay = convConfig->GetWedgeDelay();
                    double digitizingDelay = convConfig->GetDigitizingDelay();
                    double digitizingLength = convConfig->GetDigitizingLength();

                    auto digitizingSettings = convConfig->GetDigitizingSettings();
                    int compressionFactor = digitizingSettings->GetTimeSettings()->GetAscanCompressionFactor();

                    auto filterSettings = digitizingSettings->GetFilterSettings();
                    bool smoothingEnabled = filterSettings->IsSmoothingFilterEnabled();
                    double smoothingFilter = filterSettings->GetSmoothingFilter();

                    ReadThicknessSettings(convConfig->GetThicknessSettings());

                    auto gateConfigs = convConfig->GetGateConfigurations();
                    for (size_t gateIdx{}; gateIdx < gateConfigs->GetCount(); gateIdx++)
                    {
                        auto gateConfig = gateConfigs->GetGateConfiguration(gateIdx);
                        wstring name = gateConfig->GetName();
                        double samplingResol = gateConfig->GetCscanSamplingResolution();
                        bool inCycleData = gateConfig->InCycleData();
                        bool isReserveBuffer = gateConfig->IsReserveCscanBuffer();
                        GatePlacement placement = gateConfig->GetPlacement();
                    }
                }
                else if (auto paConfig = dynamic_pointer_cast<const IPhasedArrayConfiguration>(config))
                {
                    auto jsonString = paConfig->GetApplicationSettings("MyApplication");

                    IInspectionMethodPhasedArrayPtr paMethod = std::dynamic_pointer_cast<IInspectionMethodPhasedArray>(paConfig->GetInspectionMethod());
                    wstring usage = paMethod->GetUsage();
                    wstring beamSetName = paConfig->GetInspectionMethod()->GetName();

                    double velocity = paConfig->GetVelocity();

                    double gain = paConfig->GetGain();
                    double analysisGain = paConfig->GetAnalysisGain();

                    double refGain{};
                    if (paConfig->HasReferenceGain())
                        refGain = paConfig->GetReferenceGain();

                    auto digitizingSettings = paConfig->GetDigitizingSettings();
                    auto timeSettings = digitizingSettings->GetTimeSettings();

                    ITimeSettings::AscanSynchroMode ascanSynchroMode = timeSettings->GetAscanSynchroMode();

                    double wedgeDelay = paConfig->GetWedgeDelay();

                    int compressionFactor = digitizingSettings->GetTimeSettings()->GetAscanCompressionFactor();

                    auto filterSettings = digitizingSettings->GetFilterSettings();
                    bool smoothingEnabled = filterSettings->IsSmoothingFilterEnabled();
                    double smoothingFilter = filterSettings->GetSmoothingFilter();

                    ReadThicknessSettings(paConfig->GetThicknessSettings());

                    size_t beamQty = paConfig->GetBeamCount();
                    for (size_t beamIdx(0); beamIdx < paConfig->GetBeamCount(); ++beamIdx)
                    {
                        auto beam = paConfig->GetBeam(beamIdx);
                        double digitizingDelay = beam->GetDigitizingDelay();
                        double digitizingLength = beam->GetDigitizingLength();
                        double exitPointPrimary = beam->GetExitPointPrimary();
                        double skew = beam->GetSkewAngle();

                        auto gateConfigs = beam->GetGateConfigurations();
                        for (size_t gateIdx{}; gateIdx < gateConfigs->GetCount(); gateIdx++)
                        {
                            auto gateConfig = gateConfigs->GetGateConfiguration(gateIdx);
                            wstring name = gateConfig->GetName();
                            double samplingResol = gateConfig->GetCscanSamplingResolution();
                            bool inCycleData = gateConfig->InCycleData();
                            bool isReserveBuffer = gateConfig->IsReserveCscanBuffer();
                            GatePlacement placement = gateConfig->GetPlacement();
                        }
                    }
                }
                else if (auto tfmConfig = dynamic_pointer_cast<const ITotalFocusingMethodConfiguration>(config))
                {
                    auto jsonString = tfmConfig->GetApplicationSettings("MyApplication");

                    IInspectionMethodTotalFocusingPtr tfmMethod = std::dynamic_pointer_cast<IInspectionMethodTotalFocusing>(tfmConfig->GetInspectionMethod());
                    wstring usage = tfmMethod->GetUsage();
                    wstring configName = tfmConfig->GetInspectionMethod()->GetName();

                    double analysisGain = tfmConfig->GetAnalysisGain();
                }
                else {
                    throw bad_cast{};
                }
            }
        }
    }

    void Configuration::ReadThicknessSettings(IThicknessSettingsPtr thicknessSettings)
    {
        double thicknessMin = thicknessSettings->GetThicknessMin();
        double thicknessMax = thicknessSettings->GetThicknessMax();

        if (thicknessSettings->HasOneGateThicknessSource() || thicknessSettings->HasTwoGateThicknessSource())
        {
            auto thicknessSource = thicknessSettings->GetThicknessSource();
            auto gateSource = thicknessSource->GetFirstGate();
            wstring name = gateSource->GetGateName();
            EventType eventType = gateSource->GetEventType();
            AmplitudeType AmpType = gateSource->GetAmplitudeType();
        }

        if (thicknessSettings->HasTwoGateThicknessSource())
        {
            auto thicknessSource = thicknessSettings->GetThicknessSource();
            auto gateSource = thicknessSource->GetSecondGate();
            wstring name = gateSource->GetGateName();
            EventType eventType = gateSource->GetEventType();
            AmplitudeType AmpType = gateSource->GetAmplitudeType();
        }
    }

}
