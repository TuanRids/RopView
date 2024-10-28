#include "OmConnect.h"
#include "OmCreateSetupSetting/LoadLibraries.h"
#include "SystemConfig/ConfigLocator.h"
using namespace Instrumentation;
std::string wstostring(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}



OmConnect::OmConnect() : sttlogs(nullptr), acquisition(nullptr), beamSet(nullptr), datProcess(nullptr)
{
    if (!configL) configL = &ConfigLocator::getInstance();
}

bool OmConnect::omConnectDevice()
{
    
    if (!sttlogs) sttlogs = &nmainUI::statuslogs::getinstance();
    ipAddress = ConfigLocator::getInstance().settingconf->ipAddress;
    try
    {
        if (!device)
        {
            sttlogs->logNotify("Trying to Connnect to IP: " + ipAddress);
            device = DiscoverDevice();
            StartDevice();
            acquisition = IAcquisition::CreateEx(device);

            //ConfigureDevice();

            auto getsetup = OmConfigSetup::initSetup();
            auto adjusted = OmConfigSetup::ConfigDeviceFromSetup(device, getsetup);
            sttlogs->logNotify("Configuration Device From Setup: " + adjusted ? "Completed" : "Failed");
            OmConfigSetup::ConfigAcquisitionFromSetup(acquisition, getsetup);
            sttlogs->logNotify("Configuration Acquisition From Setup: " + adjusted ? "Completed" : "Failed");

            acquisition->SetRate(configL->omconf->Rate);
            acquisition->ApplyConfiguration();
        }
        if (!datProcess) datProcess = std::make_shared<nDataProcess>(acquisition);    
        if (!datProcess->Start())
        {
            acquisition.reset();
            acquisition = nullptr; ConfigureDevice();
        }

    }
    catch (const std::exception& e)
    {
        sttlogs->logCritical(e.what());
        return false;
    }
    return true;
}

void OmConnect::omDisconnectDevice()
{
    if (datProcess) datProcess->Stop();
}

shared_ptr<IDevice> OmConnect::DiscoverDevice()
{
    
    Duration timeout = ConfigLocator::getInstance().settingconf->timeout;
    auto discovery = IDeviceDiscovery::Create(ipAddress.c_str());
    DiscoverResult result = discovery->DiscoverFor(timeout);

    if (result.status != DiscoverResult::DeviceFound)
        throw std::runtime_error("# Error IP: No device were found.");
    sttlogs->logNotify("Found Device w Serial: " + result.device->GetInfo()->GetSerialNumber());
    return result.device;
}

void OmConnect::StartDevice()
{
    wstring packageName(L"FocusPxPackage");
    shared_ptr<IFirmwarePackage> package;
    auto packages = IFirmwarePackageScanner::GetFirmwarePackageCollection();
    for (size_t packageIndex(0); packageIndex < packages->GetCount(); ++packageIndex)
    {
        if (packages->GetFirmwarePackage(packageIndex)->GetName().find(packageName) != string::npos)
        {
            package = packages->GetFirmwarePackage(packageIndex);
            sttlogs->logNotify("Package: " + wstostring(packages->GetFirmwarePackage(packageIndex)->GetName()));
        }
    }
    if (package == nullptr)
        throw std::exception("Could not find the firmware package");
    if (!device->HasPackage(package))
        device->Download(package);
    device->Start(package);
    {
        HINSTANCE handle = OpenView::Libraries::LoadInstrumentation();
        auto version = Instrumentation::GetLibraryVersionEx();
        sttlogs->logNotify("Instrumentation Version: " + wstostring(version));
        OpenView::Libraries::UnLoad(handle);
        handle = OpenView::Libraries::LoadStorage();
        version = Olympus::FileManagement::Storage::GetLibraryVersion();
        sttlogs->logNotify("Storage Version: " + wstostring(version));
        OpenView::Libraries::UnLoad(handle);
    }
}
void OmConnect::ConfigureDevice()
{
    if (!device) { throw std::exception("Device not connected."); }
    std::cout << device->GetInfo();
    std::shared_ptr<IBeamSet> beamSet;
    auto ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();

    // Detect the type of ultrasound (phased array or conventional)
    auto digitizer = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
    if (digitizer)
    { beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetPhasedArray(L"BeamSet-PhasedArray", GenerateBeamFormations(digitizer->GetBeamSetFactory())); }
    else
    {
        digitizer = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::Conventional);
        beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetConventional(L"BeamSet-Conventional");
    }
    if (!digitizer)
    {
        throw std::exception("No valid ultrasound digitizer available.");
    }
    auto amplitudeSettings = beamSet->GetDigitizingSettings()->GetAmplitudeSettings();
    amplitudeSettings->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);
    for (size_t i = 0; i < beamSet->GetBeamCount(); ++i)
    {
        auto beam = beamSet->GetBeam(i);
        beam->SetAscanStart(configL->omconf->ascanStart);
        beam->SetAscanLength(configL->omconf->ascanLength);

        auto gate = beam->GetGateCollection()->GetGate(0);
        gate->SetStart(configL->omconf->gateStart);
        gate->SetLength(configL->omconf->gateLength);
        gate->SetThreshold(configL->omconf->gateThreshold);
        gate->InCycleData(true);
    }

    auto filterSettings = beamSet->GetDigitizingSettings()->GetFilterSettings();
    auto digitalBandPassFilters = digitizer->GetDigitalBandPassFilterCollection();
    auto digitalBandPassFilter = digitalBandPassFilters->GetDigitalBandPassFilter(5);
    filterSettings->SetDigitalBandPassFilter(digitalBandPassFilter);

    auto connector = digitizer->GetConnectorCollection()->GetPulseAndReceiveConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);
}

shared_ptr<IBeamFormationCollection> OmConnect::GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory)
{
    auto beamFormations = factory->CreateBeamFormationCollection();
    for (size_t beam = 0; beam < configL->omconf->beamLimit; ++beam)
    {
        auto beamFormation = factory->CreateBeamFormation(configL->omconf->elementAperture, configL->omconf->elementAperture);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        for (size_t pulser = 0; pulser < configL->omconf->elementAperture; ++pulser)
        {
            pulserDelays->GetElementDelay(pulser)->SetElementId(pulser + 1);
            pulserDelays->GetElementDelay(pulser)->SetDelay(((beam + pulser) * configL->omconf->delayResolution) + configL->omconf->pulserBaseDelay);
        }

        for (size_t receiver = 0; receiver < configL->omconf->elementAperture; ++receiver)
        {
            receiverDelays->GetElementDelay(receiver)->SetElementId(receiver + 1);
            receiverDelays->GetElementDelay(receiver)->SetDelay(((beam + receiver) * configL->omconf->delayResolution) + configL->omconf->receiverBaseDelay);
        }

        beamFormations->Add(beamFormation);
    }

    return beamFormations;
}

void OmConnect::newThread()
{
    
}
