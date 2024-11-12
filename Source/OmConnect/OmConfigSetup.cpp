#include "OmConfigSetup.h"
#include "MainUI/statuslogs.h"


OmConfigSetup::OmConfigSetup(IDevicePtr ndevice) :
    device(ndevice), ultrasoundConfig(nullptr), beamSet(nullptr), config(nullptr)
{
    if (!omSetCof) omSetCof = OmSetupL::getInstance().OMS;
    if (!ultrasoundConfig) ultrasoundConfig = ndevice->GetConfiguration()->GetUltrasoundConfiguration();

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
    for (unsigned int iBeam(0); iBeam < omSetCof->beamNumber; ++iBeam)
    {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
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
        beamFormations->Add(beamFormation);
    }
    beamSet = phasedArrayFactory->CreateBeamSetPhasedArray(L"LinearPhasedArray", beamFormations);

    // ============== Digitizing Settings
    int NbOfVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetCount();
    double HigherVoltage = 0;
    for (int VoltageIndex = 0; VoltageIndex < NbOfVoltage; VoltageIndex++)
    {
        double CurrentVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetPulserVoltage(VoltageIndex);
        if (CurrentVoltage > HigherVoltage) { HigherVoltage = CurrentVoltage; }
    }
    digitizerTechnology->SetPulserVoltage(HigherVoltage);

    // Configure for digitizing.
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

    // ============== Bind to conntector of the device
    shared_ptr<IConnector> connector = digitizerTechnology->GetConnectorCollection()->GetPulseConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);
    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->BeamGain /*+ 2.2*/);
        beamSet->GetBeam(iBeam)->SetAscanStart(omSetCof->BeamAStart);
        beamSet->GetBeam(iBeam)->SetAscanLength(omSetCof->BeamAEnd);
        // beamSet->GetBeam(iBeam)->SetSumGainMode(Instrumentation::IBeam::SumGainMode::Manual); 
        // beamSet->GetBeam(iBeam)->SetSumGain(20);
    }
    acquisition = IAcquisition::CreateEx(device);
    acquisition->SetRate(omSetCof->Rate);
    acquisition->ApplyConfiguration();
    nmainUI::statuslogs::getinstance().logInfo("Configured Setting for the device.");
	return acquisition;
}



