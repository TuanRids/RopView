#include "OmConfigSetup.h"

OmConfigSetup::OmConfigSetup(IAcquisitionPtr nacquisition, IDevicePtr ndevice, Olympus::FileManagement::ISetupPtr nsetup) :
    acquisition(nacquisition), device(ndevice), setup(nsetup), ultrasoundConfig(nullptr)
{
    if (!omSetCof) omSetCof = OmSetupL::getInstance().OMS;
    if (!ultrasoundConfig) ultrasoundConfig = ndevice->GetConfiguration()->GetUltrasoundConfiguration();
}


// Configure Device from Setup
bool IsAdjusted(IAdjustedParameterCollectionPtr adjParams)
{
    return adjParams->GetCount() > 0;
}
bool OmConfigSetup::ConfigDeviceFromSetup() {
    bool adjusted = false;
    auto inspConfig = setup->GetInspectionConfigurations()->GetConfiguration(0);

    acquisition->SetFiringTrigger(IAcquisition::FiringTrigger::Internal);

    // Set firing trigger
    switch (inspConfig->GetFiringTrigger()) {
    case FiringTrigger::Internal:
        acquisition->SetFiringTrigger(IAcquisition::FiringTrigger::Internal);
        break;
    case FiringTrigger::Encoder:
        acquisition->SetFiringTrigger(IAcquisition::FiringTrigger::Encoder);
        break;
    case FiringTrigger::External:
        acquisition->SetFiringTrigger(IAcquisition::FiringTrigger::External);
        break;
    }

    // Configure encoders
    /*auto encoders = acquisition->GetEncoders();
    auto encodersConfig = inspConfig->GetEncoderConfigurations();

    for (size_t encoderIdx = 0; encoderIdx < encoders->GetCount(); ++encoderIdx) {
        if (encodersConfig->GetCount() > encoderIdx) {
            auto encoder = encoders->GetEncoder(encoderIdx);
            auto encoderConfig = encodersConfig->GetEncoderConfiguration(encoderIdx);

            encoder->Enable(true);
            encoder->SetName(encoderConfig->GetName());
            encoder->SetResolution(static_cast<size_t>(encoderConfig->GetFiringResolution()));
            encoder->SetPresetValue(encoderConfig->GetPresetValue());

            switch (encoderConfig->GetType()) {
            case EncoderType::ClockDir:
                encoder->SetEncoderType(IEncoder::Type::ClockDir);
                break;
            case EncoderType::PulseDown:
                encoder->SetEncoderType(IEncoder::Type::PulseDown);
                break;
            case EncoderType::PulseUp:
                encoder->SetEncoderType(IEncoder::Type::PulseUp);
                break;
            case EncoderType::Quadrature:
                encoder->SetEncoderType(IEncoder::Type::Quadrature);
                break;
            }
        }
    }*/
    // Configure ultrasound settings
    ultrasoundConfig = device->GetConfiguration()->GetUltrasoundConfiguration();
    auto inspConfigs = setup->GetInspectionConfigurations();
    for (size_t inspConfigIdx = 0; inspConfigIdx < inspConfigs->GetCount(); ++inspConfigIdx) {
        auto inspConfig = inspConfigs->GetConfiguration(inspConfigIdx);
        auto configs = inspConfig->GetConfigurations();

        for (size_t configIdx = 0; configIdx < configs->GetCount(); ++configIdx) {
            auto config = configs->GetConfiguration(configIdx);
            auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(config->GetInspectionMethod());
            if (paMethod) {
                auto beamSet = Set_CreateFiringBeamSetPhasedArray();
                adjusted |= Set_ConfigBeamSetPhasedArray();
            }
        }
    }
    return adjusted;
}
IBeamSetPtr OmConfigSetup::Set_CreateFiringBeamSetPhasedArray() {
    /*auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(config->GetInspectionMethod());
    auto probePa = dynamic_pointer_cast<IProbeRectangularArray>(paMethod->GetProbe());
    auto probeDualPa = probePa ? nullptr : dynamic_pointer_cast<IProbeDualRectangularArray>(paMethod->GetProbe());
    std::wstring portName;
    if (probePa) portName = probePa->GetConnector()->GetConnection()->GetName();
    else if (probeDualPa) portName = probeDualPa->GetConnector()->GetConnection()->GetName();
    else throw bad_cast{};

    if (portName == L"PA") {*/
    auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
    auto digitizer = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    auto factory = digitizer->GetBeamSetFactory();
    auto beamFormations = factory->CreateBeamFormationCollection();

    for (size_t beamIdx = 0; beamIdx < paConfig->GetBeamCount(); ++beamIdx) {
        auto beamFormation = factory->CreateBeamFormation(
            paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetPulserDelayCollection()->GetCount(),
            paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetReceiverDelayCollection()->GetCount()
        );
        beamFormations->Add(beamFormation);
    }
    auto beamSet = factory->CreateBeamSetPhasedArray(paConfig->GetInspectionMethod()->GetName(), beamFormations);
    ultrasoundConfig->GetFiringBeamSetCollection()->Add(beamSet, digitizer->GetConnectorCollection()->GetConnector(0));
    return beamSet;
    /*}
    return nullptr;*/
}
bool OmConfigSetup::Set_ConfigBeamSetPhasedArray() {
    bool adjusted = false;
    auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);

    if (!paConfig->GetPulsingSettings()->GetPulseWidth()) {
        throw bad_cast{};
    }

    adjusted |= beamSet->GetPulsingSettings()->SetAscanAveragingFactor(paConfig->GetPulsingSettings()->GetAscanAveragingFactor());
    adjusted |= beamSet->GetPulsingSettings()->SetPulseWidth(100);

    // Digitizing Setting
    auto digitizingSettings = beamSet->GetDigitizingSettings();
    adjusted |= IsAdjusted(digitizingSettings->GetTimeSettings()->SetAscanCompressionFactor(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetAscanCompressionFactor()));
    adjusted |= IsAdjusted(digitizingSettings->GetTimeSettings()->SetAscanSynchroMode(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetAscanSynchroMode()));
    adjusted |= IsAdjusted(digitizingSettings->GetTimeSettings()->SetSamplingDecimationFactor(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetSamplingDecimationFactor()));
    adjusted |= IsAdjusted(digitizingSettings->GetTimeSettings()->SetTCGSynchroMode(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetTCGSynchroMode()));

    adjusted |= IsAdjusted(digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(paConfig->GetDigitizingSettings()->GetAmplitudeSettings()->GetAscanDataSize()));
    adjusted |= IsAdjusted(digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(paConfig->GetDigitizingSettings()->GetAmplitudeSettings()->GetAscanRectification()));
    digitizingSettings->GetAmplitudeSettings()->SetScalingType(paConfig->GetDigitizingSettings()->GetAmplitudeSettings()->GetScalingType());

    digitizingSettings->GetFilterSettings()->EnableSmoothingFilter(true);
    digitizingSettings->GetFilterSettings()->SetSmoothingFilter(ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray)->GetSmoothingFilterCollection()->GetSmoothingFilter(5.0));

    // Beam Setting
    for (size_t beamIdx = 0; beamIdx < beamSet->GetBeamCount(); ++beamIdx) {
        auto beam = beamSet->GetBeam(beamIdx);
        auto beamConfig = paConfig->GetBeam(beamIdx);

        adjusted |= IsAdjusted(beam->SetGainEx(paConfig->GetGain() + beamConfig->GetGainOffset()));
        adjusted |= IsAdjusted(beam->SetAscanStart(paConfig->GetWedgeDelay() + beamConfig->GetBeamDelay() + beamConfig->GetDigitizingDelay()));
        adjusted |= IsAdjusted(beam->SetAscanLength(beamConfig->GetDigitizingLength()));
        adjusted |= IsAdjusted(beam->SetRecurrence(beamConfig->GetRecurrence()));
        adjusted |= IsAdjusted(beam->SetSumGainMode(beamConfig->GetSumGainMode()));
        adjusted |= IsAdjusted(beam->SetSumGain(beamConfig->GetSumGain()));

        double wedgeDelay = paConfig->GetWedgeDelay() + beamConfig->GetBeamDelay();

        // Gate Setting
        for (size_t gateIdx = 0; gateIdx < beam->GetGateCollection()->GetCount(); ++gateIdx) {
            auto gate = beam->GetGateCollection()->GetGate(gateIdx);
            auto gateConfig = beamConfig->GetGateConfigurations()->GetGateConfiguration(gateIdx);
            if (gateConfig->GetName() == L"Gate Synchro") {
                gate->InCycleData(gateConfig->InCycleData());
                gate->SetAbsoluteMode(gateConfig->GetAbsoluteMode());
                gate->SetStart(wedgeDelay + gateConfig->GetDelay());
                gate->SetLength(gateConfig->GetLength());
                gate->SetThreshold(gateConfig->GetThreshold());
            }
        }

        // TCG Setting
        if (beamConfig->GetTcg()->IsEnabled()) {
            auto tcg = beam->GetTcg();
            tcg->Enable(true);
            adjusted |= IsAdjusted(tcg->SetTcgType(beamConfig->GetTcg()->GetTcgType()));

            auto tcgPoints = tcg->GetTcgPointCollection();
            for (size_t tcgIdx = 0; tcgIdx < beamConfig->GetTcg()->GetTcgPointCollection()->GetCount(); ++tcgIdx) {
                auto tcgPointConfig = beamConfig->GetTcg()->GetTcgPointCollection()->GetTcgPoint(tcgIdx);
                tcgPoints->AddTcgPointEx(wedgeDelay + tcgPointConfig->GetPosition(), tcgPointConfig->GetGain());
            }
        }

        // Pulser and Receiver Delay Configurations
        auto pulsers = beam->GetBeamFormation()->GetPulserDelayCollection();
        auto pulsersConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetPulserDelayCollection();

        for (size_t elementIdx = 0; elementIdx < pulsers->GetCount(); elementIdx += 2) {
            auto element = pulsers->GetElementDelay(elementIdx);
            auto elementConfig = pulsersConfig->GetElementDelay(elementIdx);
            element->SetElementId(elementConfig->GetElementId());
            adjusted |= element->SetDelay(elementConfig->GetDelay());
        }

        auto receivers = beam->GetBeamFormation()->GetReceiverDelayCollection();
        auto receiversConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetReceiverDelayCollection();

        for (size_t elementIdx = 0; elementIdx < receivers->GetCount(); elementIdx += 2) {
            auto element = receivers->GetElementDelay(elementIdx);
            auto elementConfig = receiversConfig->GetElementDelay(elementIdx);
            element->SetElementId(elementConfig->GetElementId());
            adjusted |= element->SetDelay(elementConfig->GetDelay());
        }
    }
    return adjusted;
}


// Configure Directly
/*void OmConfigSetup::CreateFiringBeamSetPhasedArray() {
    if (!omSetCof)
        omSetCof = OmSetupL::getInstance().OMS;

    //========================== Acquisition Unit Configuration
    auto scanPlan = setup->GetScanPlan();
    auto inspConfig = setup->GetInspectionConfigurations()->Add(FiringTrigger::Internal, 1000.);

    auto acqUnits = scanPlan->GetAcquisitionUnits();
    for (size_t acqUnitIdx(0); acqUnitIdx < acqUnits->GetCount(); acqUnitIdx++) {
        auto acqUnit = acqUnits->GetAcquisitionUnit(acqUnitIdx);
        auto acqUnitConfig = inspConfig->GetAcquisitionUnitConfigurations()->Add(acqUnit);

        auto digitizerConfig = acqUnitConfig->GetUltrasoundDigitizerConfiguration();
        auto paDigitizerConfig = digitizerConfig->GetDigitizerTechnologyConfiguration(UltrasoundTechnology::PhasedArray);
        paDigitizerConfig->SetPulserVoltage(100);
    }

    //========================== Inspection Method Configuration
    auto inspMethod = scanPlan->GetInspectionMethodCollection()->GetInspectionMethod(0); // GR-2, create from scanPlan
    auto config = inspConfig->GetConfigurations()->AddPhasedArray(dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspMethod));
    // Check why there are no setting information before from scanpplan!!!!
    // 
    // 
        // Application Settings
    //std::string applicationJsonStr = "{\"pipeWizard\": {\"Zone\": \"Volumetric\",\"Type\": \"ABC\",\"Stream\": \"Upstream\",\"GateAStart\": -3.0,\"GateALength\": 6.0,\"GateBStart\": -3.0,\"GateBLength\": 6.0,\"GateAThreshold\": 20.0,\"GateBThreshold\": 20.0,\"GateOffset\": 0.0,\"CorrectionFactor\": 0.0,\"ReflectorSize\": 1.0}}";
    //
    //config->SetApplicationSettings(applicationJsonStr);
    //config->SetVelocity(omSetCof->phasing_velocity);
    //config->SetReferenceAmplitude(omSetCof->phasing_referenceAmplitude);

        // Phased Array Configuration

    auto PAConfig_Scan = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
    auto PAConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
    auto pulsingSettings = config->GetPulsingSettings();
    pulsingSettings->SetPulseWidth(omSetCof->phasing_pulseWidth);

    PAConfig->SetVelocity(omSetCof->phasing_velocity);

    // Beam Formation Configuration
    auto digitizer = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    auto factory = digitizer->GetBeamSetFactory();
    auto beamFormations = factory->CreateBeamFormationCollection();
    for (size_t beamIdx(0); beamIdx < omSetCof->beamNumber; ++beamIdx) {
        auto beamFormation = factory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        beamFormations->Add(beamFormation);
    }

    // Firing Beam Set Creation
    auto beamSet = factory->CreateBeamSetPhasedArray(L"LinearPhasedArray", beamFormations);
    ultrasoundConfig->GetFiringBeamSetCollection()->Add(beamSet, digitizer->GetConnectorCollection()->GetConnector(0));

    // Pulsing Settings
    auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
    beamSet->GetPulsingSettings()->SetAscanAveragingFactor(paConfig->GetPulsingSettings()->GetAscanAveragingFactor());
    beamSet->GetPulsingSettings()->SetPulseWidth(100);

    // Digitizing Settings
    auto digitizingSettings = beamSet->GetDigitizingSettings();
    digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->phasing_ascanDataSize);
    digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->phasing_rectification);
    digitizingSettings->GetTimeSettings()->SetAscanCompressionFactor(1);
    digitizingSettings->GetTimeSettings()->SetAscanSynchroMode(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetAscanSynchroMode());
    digitizingSettings->GetTimeSettings()->SetSamplingDecimationFactor(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetSamplingDecimationFactor());
    digitizingSettings->GetTimeSettings()->SetTCGSynchroMode(paConfig->GetDigitizingSettings()->GetTimeSettings()->GetTCGSynchroMode());
    digitizingSettings->GetAmplitudeSettings()->SetScalingType(paConfig->GetDigitizingSettings()->GetAmplitudeSettings()->GetScalingType());
    digitizingSettings->GetFilterSettings()->EnableSmoothingFilter(true);
    digitizingSettings->GetFilterSettings()->SetSmoothingFilter(digitizer->GetSmoothingFilterCollection()->GetSmoothingFilter(5.0));

    cout << "Ascan Start: " << paConfig->GetWedgeDelay() + omSetCof->PA_beamDelay + omSetCof->PA_digitizingDelay << endl;
    // Velocity 6500m/s => 6.5ms/us
    // start: 5mm => start 5 / 6.5 = 0.769us = 769 ns
    // end: 130mm => end 130 / 6.5 = 20us = 20000 ns

    // Beam Settings

    for (size_t beamIdx = 0; beamIdx < omSetCof->beamNumber; ++beamIdx) {
        auto beam = beamSet->GetBeam(beamIdx);
        auto beamConfig = paConfig->AddBeam();

        // Configure beam parameters
        beam->SetGainEx(omSetCof->phasing_gain + 2.2);
        beam->SetAscanStart(0);
        beam->SetAscanLength(20000);
        beam->SetRecurrence(beamConfig->GetRecurrence());
        beam->SetSumGainMode(beamConfig->GetSumGainMode()); // TODO reduce samples points for faster processing
        beam->SetSumGain(beamConfig->GetSumGain());
        
        
        double wedgeDelay = paConfig->GetWedgeDelay() + beamConfig->GetBeamDelay();
        // Pulser and Receiver Delay Configurations
        auto pulsers = beam->GetBeamFormation()->GetPulserDelayCollection();
        auto receivers = beam->GetBeamFormation()->GetReceiverDelayCollection();
        auto pulsersConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetPulserDelayCollection();

        unsigned int VirAperture = omSetCof->EleFirst;
        auto NumberElem = omSetCof->EleQuantity; // wrong formula
        for (size_t elementIdx = 0; elementIdx < NumberElem; ++elementIdx) {
            pulsers->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulsers->GetElementDelay(elementIdx)->SetDelay((beamIdx + elementIdx) * omSetCof->PA_ElemInternalDelay + omSetCof->PA_elementDelay);
            receivers->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receivers->GetElementDelay(elementIdx)->SetDelay((beamIdx + elementIdx) * omSetCof->PA_ElemInternalDelay + omSetCof->PA_elementDelay * 2);
            VirAperture += omSetCof->EleStep;
        }
    }

    
}*/

void OmConfigSetup::CreateFiringBeamSetPhasedArray() {
    if (!omSetCof)
        omSetCof = OmSetupL::getInstance().OMS;

    // Configure a phased array BeamSet
    std::shared_ptr<Instrumentation::IUltrasoundConfiguration> ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();
    std::shared_ptr<Instrumentation::IDigitizerTechnology> digitizerTechnology = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray)->GetBeamSetFactory();
    

    shared_ptr<IBeamFormationCollection> beamFormations = phasedArrayFactory->CreateBeamFormationCollection();
    for (size_t iBeam(0); iBeam < omSetCof->beamNumber; ++iBeam)
    {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        unsigned int VirAperture = omSetCof->EleFirst + iBeam;
        auto NumberElem = omSetCof->EleQuantity; // wrong formula
        for (size_t elementIdx = 0; elementIdx < NumberElem; ++elementIdx) {
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay((iBeam + elementIdx) * omSetCof->PA_ElemInternalDelay + omSetCof->PA_elementDelay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay((iBeam + elementIdx) * omSetCof->PA_ElemInternalDelay + omSetCof->PA_elementDelay * 2);
            VirAperture += omSetCof->EleStep;
        }
        beamFormations->Add(beamFormation);
    }
    beamSet = phasedArrayFactory->CreateBeamSetPhasedArray(L"LinearPhasedArray", beamFormations);

    beamSet->GetDigitizingSettings()->GetAmplitudeSettings()->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);

    size_t NbOfVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetCount();
    double HigherVoltage = 0;
    for (size_t VoltageIndex = 0; VoltageIndex < NbOfVoltage; VoltageIndex++)
    {
        double CurrentVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetPulserVoltage(VoltageIndex);
        if (CurrentVoltage > HigherVoltage)
        {
            HigherVoltage = CurrentVoltage;
        }
    }

    digitizerTechnology->SetPulserVoltage(HigherVoltage);
    auto digitizingSettings = beamSet->GetDigitizingSettings();
    digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->phasing_ascanDataSize);
    digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->phasing_rectification);
    digitizingSettings->GetTimeSettings()->SetAscanCompressionFactor(1);
    digitizingSettings->GetTimeSettings()->SetAscanSynchroMode(Instrumentation::ITimeSettings::AscanSynchroMode::Absolute);
    digitizingSettings->GetTimeSettings()->SetSamplingDecimationFactor(Instrumentation::ITimeSettings::SamplingDecimationFactor::One);
    digitizingSettings->GetTimeSettings()->SetTCGSynchroMode(Instrumentation::ITimeSettings::TCGSynchroMode::RelativeAscanSynchro);
    digitizingSettings->GetAmplitudeSettings()->SetScalingType(Instrumentation::IAmplitudeSettings::ScalingType::Linear);
    digitizingSettings->GetFilterSettings()->EnableSmoothingFilter(true);
    digitizingSettings->GetFilterSettings()->SetSmoothingFilter(digitizerTechnology->GetSmoothingFilterCollection()->GetSmoothingFilter(10.0));

    // Bind the beamSet to the device
    shared_ptr<IConnector> connector = digitizerTechnology->GetConnectorCollection()->GetPulseConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);


    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        // Configure beam parameters
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->phasing_gain /*+ 2.2*/);
        // beamSet->GetBeam(iBeam)->SetAscanStart(0);
        // beamSet->GetBeam(iBeam)->SetAscanLength(20000);
        //beamSet->GetBeam(iBeam)->SetRecurrence(beamConfig->GetRecurrence());
        //beamSet->GetBeam(iBeam)->SetSumGainMode(beamConfig->GetSumGainMode()); // TODO reduce samples points for faster processing
       // beamSet->GetBeam(iBeam)->SetSumGain(beamConfig->GetSumGain());
    }

}


void OmConfigSetup::ConfigDeviceSetting()
{
    CreateFiringBeamSetPhasedArray();
}

