#include "OmConfigSetup.h"
#include "OmConnect/OmCreateSetupSetting/OpenView.ScanPlan.h"
#include "OmConnect/OmCreateSetupSetting/OpenView.Configuration.h"
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
    for (size_t iBeam(0); iBeam < omSetCof->beamNumber; ++iBeam)
    {
        auto beamFormation = beamSet->GetBeam(iBeam)->GetBeamFormation();
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        unsigned int VirAperture = omSetCof->EleFirst + iBeam;
        for (size_t elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(omSetCof->PA_elementDelay);/*(iBeam + elementIdx) * omSetCof->PA_ElemInternalDelay + */
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(omSetCof->PA_elementDelay * 2);
            VirAperture += omSetCof->EleStep;
            if (VirAperture > omSetCof->EleLast)
            {
                iBeam = omSetCof->beamNumber;
                break;
            }
        }
    }
    beamSet->GetDigitizingSettings()->GetAmplitudeSettings()->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);

    auto digitizingSettings = beamSet->GetDigitizingSettings();
    digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->phasing_ascanDataSize);
    digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->phasing_rectification);
    auto bandpass = digitizerTechnology->GetDigitalBandPassFilterCollection()->GetDigitalBandPassFilter(omSetCof->BandPassFilter);
    digitizingSettings->GetFilterSettings()->SetDigitalBandPassFilter(bandpass);

    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->phasing_gain );
        beamSet->GetBeam(iBeam)->SetAscanStart(4000);
        beamSet->GetBeam(iBeam)->SetAscanLength(35000);
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
    for (size_t iBeam(0); iBeam < omSetCof->beamNumber; ++iBeam)
    {
        shared_ptr<IBeamFormation> beamFormation = phasedArrayFactory->CreateBeamFormation(omSetCof->EleQuantity, omSetCof->EleQuantity);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        unsigned int VirAperture = omSetCof->EleFirst + iBeam;
        for (size_t elementIdx = 0; elementIdx < omSetCof->EleQuantity; ++elementIdx) {
            pulserDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            pulserDelays->GetElementDelay(elementIdx)->SetDelay(omSetCof->PA_elementDelay);/*(iBeam + elementIdx) * omSetCof->PA_ElemInternalDelay + */
            receiverDelays->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            receiverDelays->GetElementDelay(elementIdx)->SetDelay(omSetCof->PA_elementDelay * 2);
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
    size_t NbOfVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetCount();
    double HigherVoltage = 0;
    for (size_t VoltageIndex = 0; VoltageIndex < NbOfVoltage; VoltageIndex++)
    {
        double CurrentVoltage = digitizerTechnology->GetPulserVoltageCollection()->GetPulserVoltage(VoltageIndex);
        if (CurrentVoltage > HigherVoltage) { HigherVoltage = CurrentVoltage; }
    }
    digitizerTechnology->SetPulserVoltage(HigherVoltage);

    // Configure for digitizing.
    auto digitizingSettings = beamSet->GetDigitizingSettings();
    digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(omSetCof->phasing_ascanDataSize);
    digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(omSetCof->phasing_rectification);
    digitizingSettings->GetAmplitudeSettings()->SetScalingType(Instrumentation::IAmplitudeSettings::ScalingType::Linear);
    digitizingSettings->GetTimeSettings()->SetAscanCompressionFactor(10);
    digitizingSettings->GetTimeSettings()->SetAscanSynchroMode(Instrumentation::ITimeSettings::AscanSynchroMode::Absolute);
    digitizingSettings->GetTimeSettings()->SetSamplingDecimationFactor(Instrumentation::ITimeSettings::SamplingDecimationFactor::One);
    digitizingSettings->GetTimeSettings()->SetTCGSynchroMode(Instrumentation::ITimeSettings::TCGSynchroMode::RelativeAscanSynchro);
    digitizingSettings->GetFilterSettings()->EnableSmoothingFilter(true);
    digitizingSettings->GetFilterSettings()->SetSmoothingFilter(digitizerTechnology->GetSmoothingFilterCollection()->GetSmoothingFilter(4));

    auto bandpass = digitizerTechnology->GetDigitalBandPassFilterCollection()->GetDigitalBandPassFilter(omSetCof->BandPassFilter);
    bandpass->GetHighCutOffFrequency(); bandpass->GetLowCutOffFrequency(); bandpass->GetCharacteristic();
    digitizingSettings->GetFilterSettings()->SetDigitalBandPassFilter(bandpass);

    // ============== Bind to conntector of the device
    shared_ptr<IConnector> connector = digitizerTechnology->GetConnectorCollection()->GetPulseConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);
    for (size_t iBeam(0); iBeam < beamSet->GetBeamCount(); ++iBeam)
    {
        beamSet->GetBeam(iBeam)->SetGainEx(omSetCof->phasing_gain /*+ 2.2*/);
        beamSet->GetBeam(iBeam)->SetAscanStart(omSetCof->PA_AscanStart);
        beamSet->GetBeam(iBeam)->SetAscanLength(35000);
        // beamSet->GetBeam(iBeam)->SetRecurrence(100);
        // beamSet->GetBeam(iBeam)->SetSumGainMode(beamConfig->GetSumGainMode()); // TODO reduce samples points for faster processing
        // beamSet->GetBeam(iBeam)->SetSumGain(beamConfig->GetSumGain());
    }
    acquisition = IAcquisition::CreateEx(device);
    acquisition->SetRate(omSetCof->Rate);
    acquisition->ApplyConfiguration();
    nmainUI::statuslogs::getinstance().logInfo("Configured Setting for the device.");
	return acquisition;
}



