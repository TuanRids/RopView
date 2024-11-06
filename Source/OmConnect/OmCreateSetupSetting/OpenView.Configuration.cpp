#include "OpenView.Configuration.h"
std::shared_ptr<Om_Settup_Config> OpenView::Configuration::omSetCof = nullptr;

namespace OpenView
{
    void Configuration::Create(ISetupPtr setup)
    {
        if (!omSetCof)
            omSetCof = OmSetupL::getInstance().OMS;

        auto sancPlan = setup->GetScanPlan();
        auto inspConfigs = setup->GetInspectionConfigurations();
        auto inspConfig = inspConfigs->Add(FiringTrigger::Internal, 1000.);

        auto acqUnits = sancPlan->GetAcquisitionUnits();
        for (size_t acqUnitIdx(0); acqUnitIdx < acqUnits->GetCount(); acqUnitIdx++)
        {
            auto acqUnit = acqUnits->GetAcquisitionUnit(acqUnitIdx);
            auto acqUnitConfig = inspConfig->GetAcquisitionUnitConfigurations()->Add(acqUnit);
            acqUnitConfig->SetSerialNumber(omSetCof->acquisition_deviceSerialNumber);

            auto digitizerConfig = acqUnitConfig->GetUltrasoundDigitizerConfiguration();

            auto convDigitizerConfig = digitizerConfig->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::Conventional);
            double convVoltage = convDigitizerConfig->GetPulserVoltages()->GetPulserVoltage(2);
            convDigitizerConfig->SetPulserVoltage(convVoltage);

            auto paDigitizerConfig = digitizerConfig->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::PhasedArray);
            paDigitizerConfig->SetPulserVoltage(100);

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

        config->SetGain(omSetCof->phasing_gain);
        config->SetVelocity(omSetCof->phasing_velocity);
        config->SetReferenceAmplitude(omSetCof->phasing_referenceAmplitude);

        auto pulsingSettings = config->GetPulsingSettings();
        pulsingSettings->SetPulseWidth(omSetCof->phasing_pulseWidth);

        auto digitizingSettings = config->GetDigitizingSettings();
        digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->phasing_ascanDataSize);
        digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->phasing_rectification);
        for (size_t beamIdx(0); beamIdx < omSetCof->beamNumber; ++beamIdx)
        {
            auto beam = config->AddBeam();

            beam->SetBeamDelay(omSetCof->PA_beamDelay );
            beam->SetGainOffset(2.2);
            beam->SetDigitizingDelay(omSetCof->PA_digitizingDelay);
            beam->SetDigitizingLength(omSetCof->PA_DigitizingLength);
            beam->SetExitPointPrimary(omSetCof->PA_exitPoint );
            beam->SetRefractedAnglePrimary(0);
            beam->SetSkewAngle(33);

            auto beamFormation = beam->CreateBeamFormation(32, 32);            
            auto pulserDelays = beamFormation->GetPulserDelayCollection();
            auto receiverDelays = beamFormation->GetReceiverDelayCollection();

            for (size_t elementIdx(0); elementIdx < omSetCof->EleQuantity; elementIdx+= omSetCof->EleStep)
            {
                pulserDelays->GetElementDelay(elementIdx)->SetElementId(omSetCof->EleFirst + elementIdx);
                pulserDelays->GetElementDelay(elementIdx)->SetDelay( (beamIdx + elementIdx) * 2.5 + omSetCof->PA_elementDelay); /*omSetCof->PA_elementDelay[elementIdx]*/

                receiverDelays->GetElementDelay(elementIdx)->SetElementId(omSetCof->EleFirst + elementIdx);
                receiverDelays->GetElementDelay(elementIdx)->SetDelay( (beamIdx + elementIdx) * 2.5 + omSetCof->PA_elementDelay * 2 );
            }

            auto gateConfig = beam->GetGateConfigurations();

            auto gateI = gateConfig->Add(GATE_I);
            gateI->SetDelay(omSetCof->gate_gateIDelay);
            gateI->SetLength(omSetCof->gate_gateILength);
            gateI->SetThreshold(omSetCof->gate_gateIThreshold);
            gateI->SetCscanSamplingResolution(omSetCof->gate_gateASamplingResolution);

            auto gateA = gateConfig->Add(GATE_A);
            gateA->SetDelay(omSetCof->gate_gateADelay);
            gateA->SetLength(omSetCof->gate_gateALength);
            gateA->SetThreshold(omSetCof->gate_gateAThreshold);
            gateA->ReserveCscanBuffer(omSetCof->gate_gateAReserveBuffer);
            gateA->SetCscanSamplingResolution(omSetCof->gate_gateASamplingResolution);
            gateA->SetPlacement(omSetCof->gate_gateAPlacement);

            auto gateB = gateConfig->Add(GATE_B);
            gateB->SetDelay(omSetCof->gate_gateBDelay);
            gateB->SetLength(omSetCof->gate_gateBLength);
            gateB->SetThreshold(omSetCof->gate_gateBThreshold);
            gateB->ReserveCscanBuffer(omSetCof->gate_gateBReserveBuffer);
            gateB->SetCscanSamplingResolution(omSetCof->gate_gateBSamplingResolution);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

}
