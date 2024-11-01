#include "OmConfigSetup.h"

using namespace std::experimental::filesystem;
using namespace Instrumentation;
using namespace Olympus::Equipment;
using namespace Olympus::Inspection;
using namespace Olympus::FileManagement;
using namespace Olympus::FileManagement::Storage;



Olympus::FileManagement::ISetupPtr OmConfigSetup::initSetup()
{ 
    std::filesystem::path inputFile = getFilePath();
    // check if input file exists
    if (std::filesystem::exists(inputFile)) 
    {
        auto setupFile = Olympus::FileManagement::OpenSetupFile(inputFile);
        auto setup = setupFile->GetSetup();
        return setup;
    }
    else 
        throw std::exception("Input file does not exist.");
}

bool IsAdjusted(IAdjustedParameterCollectionPtr adjParams)
{
    return adjParams->GetCount() > 0;
}
// Firing Beamset Phased Array
shared_ptr<IBeamSet> Set_AddPhasedArrayFiringBeamSet(shared_ptr<IUltrasoundConfiguration> ultrasoundConfig, IConfigurationPtr config, const wstring& portName)
{
    shared_ptr<IBeamSet> beamSet;

    if (portName == L"PA")
    {
        auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
        auto digitizer = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
        
        auto factory = digitizer->GetBeamSetFactory();
        auto beamFormations = factory->CreateBeamFormationCollection();

        for (size_t beamIdx(0); beamIdx < paConfig->GetBeamCount(); ++beamIdx)
        {
            auto pulsers = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetPulserDelayCollection();
            auto receivers = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetReceiverDelayCollection();

            auto beamFormation = factory->CreateBeamFormation(pulsers->GetCount(), receivers->GetCount());
            beamFormations->Add(beamFormation);
        }

        wstring beamSetName = paConfig->GetInspectionMethod()->GetName();
        beamSet = factory->CreateBeamSetPhasedArray(beamSetName, beamFormations);

        auto connector = digitizer->GetConnectorCollection()->GetConnector(0);
        ultrasoundConfig->GetFiringBeamSetCollection()->Add(beamSet, connector);
    }

    return beamSet;
}
IBeamSetPtr Set_CreateFiringBeamSetPhasedArray(IConfigurationPtr config, shared_ptr<IUltrasoundConfiguration> ultrasoundConfig)
{
    wstring portName;
    IBeamSetPtr beamSet;

    auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(config->GetInspectionMethod());
    auto probePa = dynamic_pointer_cast<IProbeRectangularArray>(paMethod->GetProbe());
    if (probePa)
    {
        portName = probePa->GetConnector()->GetConnection()->GetName();
        
    }
    else
    {
        auto probeDualPa = dynamic_pointer_cast<IProbeDualRectangularArray>(paMethod->GetProbe());
        if (probeDualPa)
        {
            portName = probeDualPa->GetConnector()->GetConnection()->GetName();
        }
        else
            throw bad_cast{};
    }

    return Set_AddPhasedArrayFiringBeamSet(ultrasoundConfig, config, portName);
}

// Config Beamset Phased Array
bool Set_CopyDigitizingSettings(shared_ptr<IDigitizingSettings> digitizingSettings, shared_ptr<IDigitizingSettings> digitizingSettingsConfig, shared_ptr<IUltrasoundConfiguration> ultrasoundConfig)
{
    bool adjusted(false);

    auto timeSettings = digitizingSettings->GetTimeSettings();
    auto timeSettingsConfig = digitizingSettingsConfig->GetTimeSettings();
    adjusted |= IsAdjusted(timeSettings->SetAscanCompressionFactor(timeSettingsConfig->GetAscanCompressionFactor()));
    adjusted |= IsAdjusted(timeSettings->SetAscanSynchroMode(timeSettingsConfig->GetAscanSynchroMode()));
    adjusted |= IsAdjusted(timeSettings->SetSamplingDecimationFactor(timeSettingsConfig->GetSamplingDecimationFactor()));
    adjusted |= IsAdjusted(timeSettings->SetTCGSynchroMode(timeSettingsConfig->GetTCGSynchroMode()));

    auto ampSettings = digitizingSettings->GetAmplitudeSettings();
    auto ampSettingsConfig = digitizingSettingsConfig->GetAmplitudeSettings();
    adjusted |= IsAdjusted(ampSettings->SetAscanDataSize(ampSettingsConfig->GetAscanDataSize()));
    adjusted |= IsAdjusted(ampSettings->SetAscanRectification(ampSettingsConfig->GetAscanRectification()));
    ampSettings->SetScalingType(ampSettingsConfig->GetScalingType());

    auto filterSettings = digitizingSettings->GetFilterSettings();
    digitizingSettings->GetFilterSettings()->EnableSmoothingFilter(true);
    auto digitizerTechnology = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);

    auto digitizer = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    auto smoothingFilterCollection = digitizer->GetSmoothingFilterCollection();

    double probeFrequency = 5.0; 
    
    filterSettings->EnableSmoothingFilter(true);
    filterSettings->SetSmoothingFilter(smoothingFilterCollection->GetSmoothingFilter(probeFrequency));

    return adjusted;
}
bool Set_CopyGateSettings(shared_ptr<IGateCollection> gates, shared_ptr<IGateConfigurationCollection> gatesConfigs, double wedgeDelay)
{
    bool adjusted(false);

    for (size_t gateIdx(0); gateIdx < gates->GetCount(); ++gateIdx)
    {
        auto gate = gates->GetGate(gateIdx);
        auto gateConfig = gatesConfigs->GetGateConfiguration(gateIdx);
        if (gateConfig->GetName() == L"Gate Synchro")
        {
            gate->InCycleData(gateConfig->InCycleData());
            gate->SetAbsoluteMode(gateConfig->GetAbsoluteMode());
            gate->SetStart(wedgeDelay + gateConfig->GetDelay());
            gate->SetLength(gateConfig->GetLength());
            gate->SetThreshold(gateConfig->GetThreshold());
        }
    }

    return adjusted;
}
bool Set_CopTcgSettings(shared_ptr<ITcg> tcg, shared_ptr<ITcg> tcgConfig, double wedgeDelay)
{
    bool adjusted(false);

    if (tcgConfig->IsEnabled())
    {
        tcg->Enable(true);
        adjusted |= IsAdjusted(tcg->SetTcgType(tcgConfig->GetTcgType()));

        auto tcgPoints = tcg->GetTcgPointCollection();
        auto tcgPointsConfig = tcgConfig->GetTcgPointCollection();

        for (size_t tcgIdx(0); tcgIdx < tcgPointsConfig->GetCount(); ++tcgIdx)
        {
            auto tcgPointConfig = tcgPointsConfig->GetTcgPoint(tcgIdx);
            tcgPoints->AddTcgPointEx(wedgeDelay + tcgPointConfig->GetPosition(), tcgPointConfig->GetGain());
        }
    }

    return adjusted;
}
bool Set_ConfigBeamSetPhasedArray(shared_ptr<IBeamSet> beamSet, IConfigurationPtr config, shared_ptr<IUltrasoundConfiguration> ultrasoundConfig)
{
    bool adjusted(false);
    auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
    if (!(paConfig->GetPulsingSettings()->GetPulseWidth()))
        throw bad_cast{};
    else
        std::cout << paConfig->GetPulsingSettings()->GetPulseWidth() << std::endl;
    adjusted |= beamSet->GetPulsingSettings()->SetAscanAveragingFactor(paConfig->GetPulsingSettings()->GetAscanAveragingFactor());
    adjusted |= beamSet->GetPulsingSettings()->SetPulseWidth(100);
    adjusted |= Set_CopyDigitizingSettings(beamSet->GetDigitizingSettings(), paConfig->GetDigitizingSettings(), ultrasoundConfig);

    for (size_t beamIdx(0); beamIdx < beamSet->GetBeamCount(); ++beamIdx)
    {
        auto beam = beamSet->GetBeam(beamIdx);
        auto beamConfig = paConfig->GetBeam(beamIdx);

        adjusted |= IsAdjusted(beam->SetGainEx(paConfig->GetGain() + beamConfig->GetGainOffset()));
        adjusted |= IsAdjusted(beam->SetAscanStart(paConfig->GetWedgeDelay() + beamConfig->GetBeamDelay() + beamConfig->GetDigitizingDelay()));
        adjusted |= IsAdjusted(beam->SetAscanLength(beamConfig->GetDigitizingLength()));
        adjusted |= IsAdjusted(beam->SetRecurrence(beamConfig->GetRecurrence()));
        adjusted |= IsAdjusted(beam->SetSumGainMode(beamConfig->GetSumGainMode()));
        adjusted |= IsAdjusted(beam->SetSumGain(beamConfig->GetSumGain()));

        double wedgeDelay = paConfig->GetWedgeDelay() + beamConfig->GetBeamDelay();
        adjusted |= Set_CopyGateSettings(beam->GetGateCollection(), beamConfig->GetGateConfigurations(), wedgeDelay);
        adjusted |= Set_CopTcgSettings(beam->GetTcg(), beamConfig->GetTcg(), wedgeDelay);

        auto pulsers = beam->GetBeamFormation()->GetPulserDelayCollection();
        auto pulsersConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetPulserDelayCollection();

        for (size_t elementIdx(0); elementIdx < pulsers->GetCount(); elementIdx += 2 )
        {
            auto element = pulsers->GetElementDelay(elementIdx);
            auto elementConfig = pulsersConfig->GetElementDelay(elementIdx);
            element->SetElementId(elementConfig->GetElementId());
            adjusted |= element->SetDelay(elementConfig->GetDelay());
        }

        auto receivers = beam->GetBeamFormation()->GetReceiverDelayCollection();
        auto receiversConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetReceiverDelayCollection();

        for (size_t elementIdx(0); elementIdx < receivers->GetCount(); elementIdx += 2)
        {
            auto element = receivers->GetElementDelay(elementIdx);
            auto elementConfig = receiversConfig->GetElementDelay(elementIdx);
            element->SetElementId(elementConfig->GetElementId());
            adjusted |= element->SetDelay(elementConfig->GetDelay());
        }
    }

    return adjusted;
}


bool OmConfigSetup::ConfigAcquisitionFromSetup(IAcquisitionPtr nacquisition, Olympus::FileManagement::ISetupPtr setup)
{
    bool adjusted(false);

    auto inspConfigs = setup->GetInspectionConfigurations();
    auto inspConfig = inspConfigs->GetConfiguration(0);

    if (inspConfig->GetFiringTrigger() == FiringTrigger::Internal)
        nacquisition->SetFiringTrigger(IAcquisition::FiringTrigger::Internal);
    else if (inspConfig->GetFiringTrigger() == FiringTrigger::Encoder)
        nacquisition->SetFiringTrigger(IAcquisition::FiringTrigger::Encoder);
    else if (inspConfig->GetFiringTrigger() == FiringTrigger::External)
        nacquisition->SetFiringTrigger(IAcquisition::FiringTrigger::External);

    adjusted |= nacquisition->SetRate(inspConfig->GetRate());

    auto encoders = nacquisition->GetEncoders();
    auto encodersConfig = inspConfig->GetEncoderConfigurations();

    for (size_t encoderIdx(0); encoderIdx < encoders->GetCount(); ++encoderIdx)
    {
        if (encodersConfig->GetCount() >= encoderIdx + 1)
        {
            auto encoder = encoders->GetEncoder(encoderIdx);
            auto encoderConfig = encodersConfig->GetEncoderConfiguration(encoderIdx);

            encoder->Enable(true);
            encoder->SetName(encoderConfig->GetName());
            encoder->SetResolution(static_cast<size_t>(encoderConfig->GetFiringResolution()));
            encoder->SetPresetValue(encoderConfig->GetPresetValue());

            if (encoderConfig->GetType() == EncoderType::ClockDir)
                encoder->SetEncoderType(IEncoder::Type::ClockDir);
            else if (encoderConfig->GetType() == EncoderType::PulseDown)
                encoder->SetEncoderType(IEncoder::Type::PulseDown);
            else if (encoderConfig->GetType() == EncoderType::PulseUp)
                encoder->SetEncoderType(IEncoder::Type::PulseUp);
            else if (encoderConfig->GetType() == EncoderType::Quadrature)
                encoder->SetEncoderType(IEncoder::Type::Quadrature);
        }
    }
    return adjusted;
}
bool OmConfigSetup::ConfigDeviceFromSetup(IDevicePtr ndevice, Olympus::FileManagement::ISetupPtr setup)
{
    bool adjusted(false);
    auto ultrasoundConfig = ndevice->GetConfiguration()->GetUltrasoundConfiguration();

    auto inspConfigs = setup->GetInspectionConfigurations();
    for (size_t inspConfigIdx(0); inspConfigIdx < inspConfigs->GetCount(); ++inspConfigIdx)
    {
        auto inspConfig = inspConfigs->GetConfiguration(inspConfigIdx);
        auto configs = inspConfig->GetConfigurations();

        for (size_t configIdx(0); configIdx < configs->GetCount(); ++configIdx)
        {
            auto config = configs->GetConfiguration(configIdx);
            auto inspMethod = config->GetInspectionMethod();
            auto convMethod = dynamic_pointer_cast<IInspectionMethodConventional>(inspMethod);
            if (!convMethod)
            {
                auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspMethod);
                if (paMethod)
                {
                    auto beamSet = Set_CreateFiringBeamSetPhasedArray(config, ultrasoundConfig);
                    adjusted |= Set_ConfigBeamSetPhasedArray(beamSet, config, ultrasoundConfig);

                }
                else
                    throw bad_cast{};
            }
        }
    }

    return adjusted;
}
