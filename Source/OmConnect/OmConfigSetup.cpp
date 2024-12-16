#include "OmConfigSetup.h"
#include "MainUI/statuslogs.h"


OmConfigSetup::OmConfigSetup(IDevicePtr ndevice) :
    device(ndevice), beamSet(nullptr), debugLogger(nullptr)
{
    if (!omSetCof) omSetCof = OmSetupL::getInstance().OMS;
    //if (!ultrasoundConfig) ultrasoundConfig = ndevice->GetConfiguration()->GetUltrasoundConfiguration();
    debugLogger = spdlog::get("file_logger");
}

void OmConfigSetup::ConfigUpdateSetting()
{
    if (!omSetCof) { omSetCof = OmSetupL::getInstance().OMS; }
    if (!beamSet) { throw std::exception("BeamSet not configured"); }
    // Configure a phased array BeamSet
    IUltrasoundConfigurationPtr ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();
    IDigitizerTechnologyPtr digitizerTechnology = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory = digitizerTechnology->GetBeamSetFactory();

    shared_ptr<IBeamFormationCollection> beamFormations = phasedArrayFactory->CreateBeamFormationCollection();
    for (unsigned int iBeam(0); iBeam < omSetCof->beamNumber; ++iBeam)
    {
        auto beamFormation = beamSet->GetBeam(iBeam)->GetBeamFormation();
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        unsigned int VirAperture = omSetCof->EleFirst + iBeam;
        for (unsigned int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(omSetCof->Ele_Delay);/*(iBeam + elementIdx) * omSetCof->PA_ElemInternalDelay + */
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(omSetCof->Ele_Delay * 2);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast)
            {
                iBeam = omSetCof->beamNumber;
                break;
            }
        }
    }
    beamSet->GetDigitizingSettings()->GetAmplitudeSettings()->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);

    // Configure for digitizing.
    auto digitizingSettings = beamSet->GetDigitizingSettings();
    digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->Digi_Ampli_AscanSize);
    digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->Digi_Ampli_rectification);
    digitizingSettings->GetAmplitudeSettings()->SetScalingType(omSetCof->Digi_Ampli_Scaling);
    digitizingSettings->GetTimeSettings()->SetAscanCompressionFactor(omSetCof->Digi_Time_CompressFactor);
    digitizingSettings->GetTimeSettings()->SetAscanSynchroMode(omSetCof->Digi_Time_Synch);
    digitizingSettings->GetTimeSettings()->SetSamplingDecimationFactor(omSetCof->Digi_Time_DecimaFactor);

    auto bandpass = digitizerTechnology->GetDigitalBandPassFilterCollection()->GetDigitalBandPassFilter(omSetCof->Digi_BandPassFilter);
    bandpass->GetHighCutOffFrequency(); bandpass->GetLowCutOffFrequency(); bandpass->GetCharacteristic();
    digitizingSettings->GetFilterSettings()->SetDigitalBandPassFilter(bandpass);

    double calAscanStart = 2 * omSetCof->BeamAStart * 1e6 / omSetCof->Velocity;
    double calAscanLength = 2 * omSetCof->BeamAEnd * 1e6 / omSetCof->Velocity;
    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->BeamGain /*+ 2.2*/);
        beamSet->GetBeam(iBeam)->SetAscanStart(calAscanStart);
        beamSet->GetBeam(iBeam)->SetAscanLength(calAscanLength);
    }
    acquisition->SetRate(omSetCof->Rate);
}
void OmConfigSetup::removeAcquisition()
{
    if (acquisition)
    {
        std::cout << "Acquistion number: " << acquisition.use_count() << std::endl;
        acquisition.reset(); acquisition = nullptr;
    }
}


std::string OmConfigSetup::appendLog(std::string& logger, unsigned int VirAperture, double delayNoFocus = 0, double deltaN = 0, double delay = 0) {
    if (omSetCof->FocusLength == 0) {
        logger += fmt::format("{:>5} {:>10.3f}\n", VirAperture, delay);
    }
    else logger += fmt::format("{:>5} {:>10.3f} {:>10.3f} {:>10.3f}\n", VirAperture, delayNoFocus, deltaN, delay);
    return logger;
}

IAcquisitionPtr OmConfigSetup::ConfigDeviceSetting()
{
    if (!omSetCof) { omSetCof = OmSetupL::getInstance().OMS; }

    // ============== Configure a phased array BeamSet
    IUltrasoundConfigurationPtr ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();
    IDigitizerTechnologyPtr digitizerTechnology = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory = digitizerTechnology->GetBeamSetFactory();

    shared_ptr<IBeamFormationCollection> beamFormations = phasedArrayFactory->CreateBeamFormationCollection();

    switch (ConfigLocator::getInstance().visualConfig->setPautMode) {
    case PautModeOmni::Linear:
        ConfigureLinearBeam(phasedArrayFactory, beamFormations);
        break;
    case PautModeOmni::Sectorial:
        ConfigureSectorialBeam(phasedArrayFactory, beamFormations);
        break;
    case PautModeOmni::Compound:
        ConfigureCompoundBeam(phasedArrayFactory, beamFormations);
        break;
    case PautModeOmni::TFM:
        ConfigureCompoundBeam(phasedArrayFactory, beamFormations);
        break;
    default:
        throw std::runtime_error("Unknown PAUT mode");
    }
    beamSet = nullptr;
    beamSet = phasedArrayFactory->CreateBeamSetPhasedArray(L"LinearPhasedArray", beamFormations);

    // Configure for digitizing.
    ConfigureDigitizingSettings(digitizerTechnology);


    // ============== Bind to conntector of the device
    shared_ptr<IConnector> connector = digitizerTechnology->GetConnectorCollection()->GetPulseConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);
    double calAscanStart = 2 * omSetCof->BeamAStart * 1e6 / omSetCof->Velocity;
    double calAscanLength = 2 * omSetCof->BeamAEnd * 1e6 / omSetCof->Velocity;

    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->BeamGain /*+ 2.2*/);
        beamSet->GetBeam(iBeam)->SetAscanStart(calAscanStart);
        beamSet->GetBeam(iBeam)->SetAscanLength(calAscanLength);
    }

    acquisition = IAcquisition::CreateEx(device);

    acquisition->SetRate(omSetCof->Rate);
    acquisition->ApplyConfiguration();
    nmainUI::statuslogs::getinstance().logInfo("Configured Setting for the device.");
    return acquisition;
}

void OmConfigSetup::ConfigureLinearBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations)
{
    std::string logger = fmt::format("Configure: LinearBeam\n");
    logger += std::string(40, '-') + "\n";
    logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "VirA", "delay_nF", "Delta_n", "Final Delay");

    double focus = omSetCof->FocusLength; // m
    if (focus > 0) // If Focus, the number of elements should be odd
    {
        if (omSetCof->EleQuantity % 2 == 0) {
            if (omSetCof->EleQuantity < 32) omSetCof->EleQuantity += 1;
            else if (omSetCof->EleQuantity >= 32) omSetCof->EleQuantity -= 1;
        }
    }
    int mid_elem = 0;
    if (omSetCof->EleQuantity > 2 && mid_elem == 0) { mid_elem = (omSetCof->EleQuantity - 1) / 2; }
    for (unsigned int iBeam = 0; iBeam < omSetCof->beamNumber; iBeam++) {
        logger += fmt::format("iBeam: {}\n", iBeam);
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
        double delay_noFocus = 0, delta_n = 0;
        double delay = 0 ;

        unsigned int VirAperture = omSetCof->EleFirst + iBeam;

        for (int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) { // from 0 to 16
            if (focus == 0) { delay = elementIdx * 0.6 * 1e6 * sin(omSetCof->BeamStartAngle * M_PI / 180) / (6500);             }
            else { // ************* Focus
                delay_noFocus = elementIdx * 0.6 * 1e6 * sin(omSetCof->BeamStartAngle * M_PI / 180) / (6500);
                int nth =/* mid_elem -*/ std::abs(elementIdx - mid_elem);
                delta_n = (nth * nth * 0.6 * 0.6 * 1e6) / (2 * focus * omSetCof->Velocity);
                delay = delay_noFocus - delta_n;
            }
            delay += 500;
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(delay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(500 + delay);
            appendLog(logger, VirAperture, delay_noFocus, delta_n, delay);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast) {
                iBeam = omSetCof->beamNumber;
                break;
            }
        }
        beamFormations->Add(beamFormation);
    }
    debugLogger->debug(logger + std::string(40, '-') + "\n");
}

void OmConfigSetup::ConfigureSectorialBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations)
{
    std::string logger = fmt::format("Configure: SectorialBeam\n");
    logger += std::string(40, '-') + "\n";
    logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "VirA", "delay_nF", "Delta_n", "Final Delay");

    double focus = omSetCof->FocusLength; // m
    if (focus > 0) // If Focus, the number of elements should be odd
    {
        if (omSetCof->EleQuantity % 2 == 0) {
            if (omSetCof->EleQuantity < 32) omSetCof->EleQuantity += 1;
            else if (omSetCof->EleQuantity >= 32) omSetCof->EleQuantity -= 1;
        }
    }
    int mid_elem = 0;
    if (omSetCof->EleQuantity > 2 && mid_elem == 0) { mid_elem = (omSetCof->EleQuantity - 1) / 2; }
    for (unsigned int iBeam = 0; iBeam < omSetCof->beamNumber; ++iBeam) {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
        unsigned int VirAperture = omSetCof->EleFirst;
        double delay_noFocus = 0, delta_n = 0;
        logger += fmt::format("iBeam: {}\n", iBeam);
        for (int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            double delay{ 0 };
            if (focus == 0) { delay = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500); }
            else { // ************* Focus
                delay_noFocus = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500);
                int nth =/* mid_elem -*/ std::abs(elementIdx - mid_elem);
                delta_n = (nth * nth * 0.6 * 0.6 * 1e6) / (2 * focus * omSetCof->Velocity);
                delay = delay_noFocus - delta_n;
            }
            delay += 500;
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(delay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(500 + delay);
            appendLog(logger, VirAperture, delay_noFocus, delta_n, delay);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast) {
                iBeam = omSetCof->beamNumber;
                break;
            }
        }
        beamFormations->Add(beamFormation);
    }
    debugLogger->debug(logger + std::string(40, '-') + "\n");
}

void OmConfigSetup::ConfigureCompoundBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations)
{
    std::string logger = fmt::format("Configure: Compound\n");
    logger += std::string(40, '-') + "\n";
    logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "VirA", "delay_nF", "Delta_n", "Final Delay");

    double focus = omSetCof->FocusLength; // m
    if (focus > 0) // If Focus, the number of elements should be odd
    {
        if (omSetCof->EleQuantity % 2 == 0) {
            if (omSetCof->EleQuantity < 32) omSetCof->EleQuantity += 1;
            else if (omSetCof->EleQuantity >= 32) omSetCof->EleQuantity -= 1;
        }
    }
    int mid_elem = 0;
    if (omSetCof->EleQuantity > 2 && mid_elem == 0) { mid_elem = (omSetCof->EleQuantity - 1) / 2; }
    for (unsigned int iBeam = 0; iBeam < omSetCof->beamNumber; ++iBeam) {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
        unsigned int VirAperture = omSetCof->EleFirst + iBeam;
        double delay_noFocus = 0, delta_n = 0;
        logger += fmt::format("iBeam: {}\n", iBeam);
        for (int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            double delay{ 0 };
            if (focus == 0) { delay = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500); }
            else { // ************* Focus
                delay_noFocus = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500);
                int nth =/* mid_elem -*/ std::abs(elementIdx - mid_elem);
                delta_n = (nth * nth * 0.6 * 0.6 * 1e6) / (2 * focus * omSetCof->Velocity);
                delay = delay_noFocus - delta_n;
            }
            delay += 500;
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(delay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(500 + delay);
            appendLog(logger, VirAperture, delay_noFocus, delta_n, delay);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast) {
                iBeam = omSetCof->beamNumber;
                break;
            }
        }
        beamFormations->Add(beamFormation);
    }
    debugLogger->debug(logger + std::string(40, '-') + "\n");
}

void OmConfigSetup::ConfigureTFM(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations) {
    std::string logger = fmt::format("Configure: TFM\n");
    logger += std::string(40, '-') + "\n";
    logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "Pulser", "Receiver", "Delay", "Final");

    for (unsigned int iPulser = 0; iPulser < omSetCof->EleQuantity; ++iPulser) {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(1, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        pulserDelays->GetElementDelay(0)->SetElementId(iPulser);
        pulserDelays->GetElementDelay(0)->SetDelay(0); 
        for (unsigned int iReceiver = 0; iReceiver < omSetCof->EleQuantity; ++iReceiver) {
            receiverDelays->GetElementDelay(iReceiver)->SetElementId(iReceiver);
            receiverDelays->GetElementDelay(iReceiver)->SetDelay(0);
            logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", iPulser, iReceiver, 0, 0);
        }

        beamFormations->Add(beamFormation);
    }

    debugLogger->debug(logger + std::string(40, '-') + "\n");
}

void OmConfigSetup::ConfigureDigitizingSettings(IDigitizerTechnologyPtr digitizerTechnology)
{
    // ============== Digitizing Settings
    size_t NbOfVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetCount();
    double HigherVoltage = 0;
    for (size_t VoltageIndex = 0; VoltageIndex < NbOfVoltage; VoltageIndex++)
    {
        double CurrentVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetPulserVoltage(VoltageIndex);
        if (CurrentVoltage > HigherVoltage) { HigherVoltage = CurrentVoltage; }
    }
    digitizerTechnology->SetPulserVoltage(HigherVoltage);

    Instrumentation::IDigitizingSettingsPtr digitizingSettings = beamSet->GetDigitizingSettings();
    digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->Digi_Ampli_AscanSize);
    digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->Digi_Ampli_rectification);
    digitizingSettings->GetAmplitudeSettings()->SetScalingType(omSetCof->Digi_Ampli_Scaling);
    digitizingSettings->GetTimeSettings()->SetAscanCompressionFactor(omSetCof->Digi_Time_CompressFactor);
    digitizingSettings->GetTimeSettings()->SetAscanSynchroMode(omSetCof->Digi_Time_Synch);
    digitizingSettings->GetTimeSettings()->SetSamplingDecimationFactor(omSetCof->Digi_Time_DecimaFactor);
    digitizingSettings->GetFilterSettings()->EnableSmoothingFilter(true);
    digitizingSettings->GetFilterSettings()->SetSmoothingFilter(digitizerTechnology->GetSmoothingFilterCollection()->GetSmoothingFilter(4));

    auto bandpass = digitizerTechnology->GetDigitalBandPassFilterCollection()->GetDigitalBandPassFilter(omSetCof->Digi_BandPassFilter);
    bandpass->GetHighCutOffFrequency(); bandpass->GetLowCutOffFrequency(); bandpass->GetCharacteristic();
    digitizingSettings->GetFilterSettings()->SetDigitalBandPassFilter(bandpass);
}

