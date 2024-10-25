#include "OmConfigSetup.h"

using namespace std::experimental::filesystem;
using namespace Instrumentation;
using namespace Olympus::Equipment;
using namespace Olympus::Inspection;
using namespace Olympus::FileManagement;
using namespace Olympus::FileManagement::Storage;
Olympus::FileManagement::ISetupPtr OmConfigSetup::initSetup()
{ 
    std::filesystem::path inputFile = std::filesystem::current_path() / "sample" / "Setup.ovs";
    // check if input file exists
    if (std::filesystem::exists(inputFile)) 
    {
        auto setupFile = Olympus::FileManagement::OpenSetupFile(inputFile);
        auto setup = setupFile->GetSetup();
        return setup;
    }
}

shared_ptr<IBeamSet> AddConventionalFiringBeamSet(shared_ptr<IUltrasoundConfiguration> ultrasoundConfig, const wstring& beamSetName, const wstring& portName)
{
    shared_ptr<IBeamSet> beamSet;
    shared_ptr<IConnector> connector;

    if (portName == FocusPX::PA_PORT)
    {
        auto digitizer = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
        beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetConventional(beamSetName);
        connector = digitizer->GetConnectorCollection()->GetConnector(0);
    }
    else
    {
        auto digitizer = ultrasoundConfig->GetDigitizerTechnology(UltrasoundTechnology::Conventional);
        beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetConventional(beamSetName);

        if (portName == FocusPX::UT_P1_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(0);
        }
        else if (portName == FocusPX::UT_P2_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(1);
        }
        else if (portName == FocusPX::UT_P3_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(2);
        }
        else if (portName == FocusPX::UT_P4_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(3);
        }
        else if (portName == FocusPX::UT_P2R2_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(4);
        }
        else if (portName == FocusPX::UT_P3R3_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(5);
        }
        else if (portName == FocusPX::UT_P3R3_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(6);
        }
        else if (portName == FocusPX::UT_P4R4_PORT)
        {
            connector = digitizer->GetConnectorCollection()->GetConnector(7);
        }
    }

    ultrasoundConfig->GetFiringBeamSetCollection()->Add(beamSet, connector);

    return beamSet;
}

shared_ptr<IBeamSet> AddPhasedArrayFiringBeamSet(shared_ptr<IUltrasoundConfiguration> ultrasoundConfig, IConfigurationPtr config, const wstring& portName)
{
    shared_ptr<IBeamSet> beamSet;

    if (portName == L"PAPort")
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

bool IsAdjusted(IAdjustedParameterCollectionPtr adjParams)
{
    return adjParams->GetCount() > 0;
}

IBeamSetPtr CreateFiringBeamSetConventional(IConfigurationPtr config, shared_ptr<IUltrasoundConfiguration> ultrasoundConfig)
{
    IBeamSetPtr beamSet;
    auto peMethod = dynamic_pointer_cast<IInspectionMethodPulseEcho>(config->GetInspectionMethod());
    if (peMethod)
    {
        auto probeConv = dynamic_pointer_cast<IProbeConventional>(peMethod->GetProbe());
        if (probeConv)
        {
            wstring portName = probeConv->GetConnector()->GetConnection()->GetName();
            beamSet = AddConventionalFiringBeamSet(ultrasoundConfig, peMethod->GetName(), portName);
        }
        else
        {
            auto probePa = dynamic_pointer_cast<IProbeRectangularArray>(peMethod->GetProbe());
            if (probePa)
            {
                wstring portName = probePa->GetConnector()->GetConnection()->GetName();
                beamSet = AddConventionalFiringBeamSet(ultrasoundConfig, peMethod->GetName(), portName);
            }
            else
                throw bad_cast{};
        }
    }
    else
    {
        auto pcMethod = dynamic_pointer_cast<IInspectionMethodPitchCatch>(config->GetInspectionMethod());
        if (pcMethod)
        {
            auto pProbe = dynamic_pointer_cast<IProbeConventional>(pcMethod->GetProbePulsing());
            auto rProbe = dynamic_pointer_cast<IProbeConventional>(pcMethod->GetProbeReceiving());
            if (pProbe && rProbe)
            {
                //TODO
            }
            else
                throw bad_cast{};
        }
        else
        {
            auto tofdMethod = dynamic_pointer_cast<IInspectionMethodTofd>(config->GetInspectionMethod());
            if (tofdMethod)
            {
                auto pProbe = dynamic_pointer_cast<IProbeConventional>(tofdMethod->GetProbePulsing());
                auto rProbe = dynamic_pointer_cast<IProbeConventional>(tofdMethod->GetProbeReceiving());
                if (pProbe && rProbe)
                {
                    //TODO
                }
                else
                    throw bad_cast{};
            }
            else
                throw bad_cast{};
        }
    }

    return beamSet;
}

IBeamSetPtr CreateFiringBeamSetPhasedArray(IConfigurationPtr config, shared_ptr<IUltrasoundConfiguration> ultrasoundConfig)
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

    return AddPhasedArrayFiringBeamSet(ultrasoundConfig, config, portName);
}

IConventionalConfigurationPtr GetConventionalConfig(ISetupPtr setup, const wstring& beamSetName)
{
    IConfigurationPtr config;
    auto inspConfigs = setup->GetInspectionConfigurations();
    for (size_t inspConfigIdx(0); inspConfigIdx < inspConfigs->GetCount(); ++inspConfigIdx)
    {
        auto inspConfig = inspConfigs->GetConfiguration(inspConfigIdx);
        auto configs = inspConfig->GetConfigurations();
        for (size_t configIdx(0); configIdx < configs->GetCount() && config == nullptr; ++configIdx)
        {
            config = configs->FindConfiguration(beamSetName);
        }
    }

    return dynamic_pointer_cast<IConventionalConfiguration>(config);;
}
bool CopyDigitizingSettings(shared_ptr<IDigitizingSettings> digitizingSettings, shared_ptr<IDigitizingSettings> digitizingSettingsConfig)
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

    /*auto filterSettings = digitizingSettings->GetFilterSettings();
    auto filterSettingsConfig = digitizingSettingsConfig->GetFilterSettings();
    filterSettings->SetDigitalBandPassFilter(filterSettingsConfig->GetDigitalBandPassFilter());

    if (filterSettingsConfig->IsSmoothingFilterEnabled())
    {
      filterSettings->EnableSmoothingFilter(true);
      filterSettings->SetSmoothingFilter(filterSettingsConfig->GetSmoothingFilter());
    }*/

    return adjusted;
}

bool CopyGateSettings(shared_ptr<IGateCollection> gates, shared_ptr<IGateConfigurationCollection> gatesConfigs, double wedgeDelay)
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

bool CopTcgSettings(shared_ptr<ITcg> tcg, shared_ptr<ITcg> tcgConfig, double wedgeDelay)
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
bool ConfigBeamSetConventional(shared_ptr<IBeamSet> beamSet, IConfigurationPtr config)
{
    bool adjusted(false);

    auto convConfig = dynamic_pointer_cast<IConventionalConfiguration>(config);
    beamSet->GetPulsingSettings()->Enable(convConfig->GetPulsingSettings()->IsEnabled());
    adjusted |= beamSet->GetPulsingSettings()->SetAscanAveragingFactor(convConfig->GetPulsingSettings()->GetAscanAveragingFactor());
    adjusted |= beamSet->GetPulsingSettings()->SetPulseWidth(convConfig->GetPulsingSettings()->GetPulseWidth());
    adjusted |= CopyDigitizingSettings(beamSet->GetDigitizingSettings(), convConfig->GetDigitizingSettings());

    auto beam = beamSet->GetBeam(0);
    adjusted |= IsAdjusted(beam->SetGainEx(convConfig->GetGain()));
    adjusted |= IsAdjusted(beam->SetAscanStart(convConfig->GetWedgeDelay() + convConfig->GetDigitizingDelay()));
    adjusted |= IsAdjusted(beam->SetAscanLength(convConfig->GetDigitizingLength()));
    adjusted |= IsAdjusted(beam->SetRecurrence(convConfig->GetRecurrence()));

    adjusted |= CopyGateSettings(beam->GetGateCollection(), convConfig->GetGateConfigurations(), convConfig->GetWedgeDelay());
    adjusted |= CopTcgSettings(beam->GetTcg(), convConfig->GetTcg(), convConfig->GetWedgeDelay());

    return adjusted;
}
bool ConfigBeamSetPhasedArray(shared_ptr<IBeamSet> beamSet, IConfigurationPtr config)
{
    bool adjusted(false);

    auto paConfig = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
    adjusted |= beamSet->GetPulsingSettings()->SetAscanAveragingFactor(paConfig->GetPulsingSettings()->GetAscanAveragingFactor());
    adjusted |= beamSet->GetPulsingSettings()->SetPulseWidth(paConfig->GetPulsingSettings()->GetPulseWidth());
    adjusted |= CopyDigitizingSettings(beamSet->GetDigitizingSettings(), paConfig->GetDigitizingSettings());

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
        adjusted |= CopyGateSettings(beam->GetGateCollection(), beamConfig->GetGateConfigurations(), wedgeDelay);
        adjusted |= CopTcgSettings(beam->GetTcg(), beamConfig->GetTcg(), wedgeDelay);

        auto pulsers = beam->GetBeamFormation()->GetPulserDelayCollection();
        auto pulsersConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetPulserDelayCollection();

        for (size_t elementIdx(0); elementIdx < pulsers->GetCount(); ++elementIdx)
        {
            auto element = pulsers->GetElementDelay(elementIdx);
            auto elementConfig = pulsersConfig->GetElementDelay(elementIdx);
            element->SetElementId(elementConfig->GetElementId());
            adjusted |= element->SetDelay(elementConfig->GetDelay());
        }

        auto receivers = beam->GetBeamFormation()->GetReceiverDelayCollection();
        auto receiversConfig = paConfig->GetBeam(beamIdx)->GetBeamFormation()->GetReceiverDelayCollection();

        for (size_t elementIdx(0); elementIdx < receivers->GetCount(); ++elementIdx)
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
            if (convMethod)
            {
                auto beamSet = CreateFiringBeamSetConventional(config, ultrasoundConfig);
                adjusted |= ConfigBeamSetConventional(beamSet, config);
            }
            else
            {
                auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspMethod);
                if (paMethod)
                {
                    auto beamSet = CreateFiringBeamSetPhasedArray(config, ultrasoundConfig);
                    adjusted |= ConfigBeamSetPhasedArray(beamSet, config);
                }
                else
                    throw bad_cast{};
            }
        }
    }

    return adjusted;
}
