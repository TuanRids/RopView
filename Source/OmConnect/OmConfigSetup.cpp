#include "OmConfigSetup.h"
#include "MainUI/statuslogs.h"


OmConfigSetup::OmConfigSetup(IDevicePtr ndevice) :
    device(ndevice), ultrasoundConfig(nullptr), beamSet(nullptr), config(nullptr), debugLogger(nullptr)
{
    if (!omSetCof) omSetCof = OmSetupL::getInstance().OMS;
    if (!ultrasoundConfig) ultrasoundConfig = ndevice->GetConfiguration()->GetUltrasoundConfiguration();
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


    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->BeamGain /*+ 2.2*/);
        beamSet->GetBeam(iBeam)->SetAscanStart(omSetCof->BeamAStart);
        beamSet->GetBeam(iBeam)->SetAscanLength(omSetCof->BeamAEnd);
    }
    acquisition->SetRate(omSetCof->Rate);
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
    default:
        throw std::runtime_error("Unknown PAUT mode");
    }

    beamSet = phasedArrayFactory->CreateBeamSetPhasedArray(L"LinearPhasedArray", beamFormations);

    // Configure for digitizing.
    ConfigureDigitizingSettings(digitizerTechnology);


    // ============== Bind to conntector of the device
    shared_ptr<IConnector> connector = digitizerTechnology->GetConnectorCollection()->GetPulseConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);
    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->BeamGain /*+ 2.2*/);
        beamSet->GetBeam(iBeam)->SetAscanStart(omSetCof->BeamAStart);
        beamSet->GetBeam(iBeam)->SetAscanLength(omSetCof->BeamAEnd);
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
    double focus = omSetCof->FocusLength; // m
    if (focus > 0 ) // If Focus, the number of elements should be odd
    {
        if (omSetCof->EleQuantity % 2 == 0) {
            if (omSetCof->EleQuantity < 32) omSetCof->EleQuantity += 1;
            else if (omSetCof->EleQuantity >= 32) omSetCof->EleQuantity -= 1;
        }
        logger += std::string(40, '-') + "\n";
        logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "VirA", "delay_nF", "Delta_n", "Final Delay");
    }
    else
    {
        logger += fmt::format("{:>5} {:>10} \n", "VirA", "delay_nF");
    }
    for (unsigned int iBeam(0); iBeam < omSetCof->beamNumber; ++iBeam) {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
        unsigned int VirAperture = omSetCof->EleFirst + iBeam;

        logger += fmt::format("iBeam: {}\n", iBeam);

        for (unsigned int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            double delay{ 0 };
            if (focus == 0) {
                delay = elementIdx * 0.6 * 1e6 * sin(omSetCof->BeamStartAngle * M_PI / 180) / (6500);
                logger += fmt::format("{:>5} {:>10.3f}\n", VirAperture, delay);
            }
            else {
                auto delay_noFocus = elementIdx * 0.6 * 1e6 * sin(omSetCof->BeamStartAngle * M_PI / 180) / (6500);
                // ************* Focus
                unsigned int mid_elem = 0;
                if (omSetCof->EleQuantity > 2 && mid_elem == 0) { mid_elem = (omSetCof->EleQuantity - 1) / 2; }
                double nth = (elementIdx > mid_elem) ? omSetCof->EleQuantity - elementIdx - 1 : elementIdx;
                double delta_n = (nth * nth * 0.6 * 0.6 * 1e6) / (2 * focus * omSetCof->Velocity);  
                if (delay_noFocus > 0) delay = delay_noFocus - delta_n; 
                logger += fmt::format("{:>5} {:>10.3f} {:>10.3f} {:>10.3f}\n", VirAperture, delay_noFocus, delta_n, delay);
            }

            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(delay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(500 + delay);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast) {
                iBeam = omSetCof->beamNumber;
                break;
            }
        } 
        beamFormations->Add(beamFormation);
    }
    logger += std::string(40, '-') + "\n";
    debugLogger->debug(logger);
}

void OmConfigSetup::ConfigureSectorialBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations)
{
    std::string logger = fmt::format("Configure: SectorialBeam\n");
    double focus = omSetCof->FocusLength; // m
    if (focus > 0) // If Focus, the number of elements should be odd
    {
        if (omSetCof->EleQuantity % 2 == 0) {
            if (omSetCof->EleQuantity < 32) omSetCof->EleQuantity += 1;
            else if (omSetCof->EleQuantity >= 32) omSetCof->EleQuantity -= 1;
        }
        logger += std::string(40, '-') + "\n";
        logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "VirA", "delay_nF", "Delta_n", "Final Delay");
    }
    else
    {
        logger += fmt::format("{:>5} {:>10} \n", "VirA", "delay_nF");
    }
    for (unsigned int iBeam = 0; iBeam < omSetCof->beamNumber; ++iBeam) {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
        unsigned int VirAperture = omSetCof->EleFirst;

        logger += fmt::format("iBeam: {}\n", iBeam);
        for (unsigned int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            double delay{ 0 }; 
            if (focus == 0) {
                delay = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500);
                logger += fmt::format("{:>5} {:>10.3f}\n", VirAperture, delay);
            }
            else {
                auto delay_noFocus = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500);
                // ************* Focus
                unsigned int mid_elem = 0;
                if (omSetCof->EleQuantity > 2 && mid_elem == 0) { mid_elem = (omSetCof->EleQuantity - 1) / 2; }
                double nth = (elementIdx > mid_elem) ? omSetCof->EleQuantity - elementIdx - 1 : elementIdx;
                double delta_n = (nth * nth * 0.6 * 0.6 * 1e6) / (2 * focus * omSetCof->Velocity);
                if (delay_noFocus > 0) delay = delay_noFocus - delta_n;
                logger += fmt::format("{:>5} {:>10.3f} {:>10.3f} {:>10.3f}\n", VirAperture, delay_noFocus, delta_n, delay);
            }

            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(delay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(500 + delay);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast) {
                iBeam = omSetCof->beamNumber;
                break;
            }
        } 
        beamFormations->Add(beamFormation);
    }
    logger += std::string(40, '-') + "\n";
    debugLogger->debug(logger);
}

void OmConfigSetup::ConfigureCompoundBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations)
{
    std::string logger = fmt::format("Configure: CompundBeam\n");
    double focus = omSetCof->FocusLength; // m
    if (focus > 0) // If Focus, the number of elements should be odd
    {
        if (omSetCof->EleQuantity % 2 == 0) {
            if (omSetCof->EleQuantity < 32) omSetCof->EleQuantity += 1;
            else if (omSetCof->EleQuantity >= 32) omSetCof->EleQuantity -= 1;
        }
        logger += std::string(40, '-') + "\n";
        logger += fmt::format("{:>5} {:>10} {:>10} {:>10}\n", "VirA", "delay_nF", "Delta_n", "Final Delay");
    }
    else
    {
        logger += fmt::format("{:>5} {:>10} \n", "VirA", "delay_nF");
    }
    for (unsigned int iBeam = 0; iBeam < omSetCof->beamNumber; ++iBeam) {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
        unsigned int VirAperture = omSetCof->EleFirst;

        for (unsigned int elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            double delay{ 0 };
            if (focus == 0) {
                delay = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500);
                logger += fmt::format("{:>5} {:>10.3f}\n", VirAperture, delay);
            }
            else {
                auto delay_noFocus = elementIdx * 0.6 * 1e6 * sin((omSetCof->BeamStartAngle + iBeam) * M_PI / 180) / (6500);
                // ************* Focus
                unsigned int mid_elem = 0;
                if (omSetCof->EleQuantity > 2 && mid_elem == 0) { mid_elem = (omSetCof->EleQuantity - 1) / 2; }
                double nth = (elementIdx > mid_elem) ? omSetCof->EleQuantity - elementIdx - 1 : elementIdx;
                double delta_n = (nth * nth * 0.6 * 0.6 * 1e6) / (2 * focus * omSetCof->Velocity);
                if (delay_noFocus > 0) delay = delay_noFocus - delta_n;
                logger += fmt::format("{:>5} {:>10.3f} {:>10.3f} {:>10.3f}\n", VirAperture, delay_noFocus, delta_n, delay);
            }

            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(delay);
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(500 + delay);
            cout << delay << " ";
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast) {
                iBeam = omSetCof->beamNumber;
                break;
            }
        } cout << endl;
        beamFormations->Add(beamFormation);
    }
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

    auto digitizingSettings = beamSet->GetDigitizingSettings();
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

