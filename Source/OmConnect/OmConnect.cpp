#include "OmConnect.h"
#include "OmCreateSetupSetting/LoadLibraries.h"
#include "SystemConfig/ConfigLocator.h"
using namespace Instrumentation;
std::string wstostring(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

#include "OmConnect/OmCreateSetupSetting/OpenView.Configuration.h"
#include "OmConnect/OmCreateSetupSetting/OpenView.ScanPlan.h"

OmConnect::OmConnect() : sttlogs(nullptr), acquisition(nullptr), datProcess(nullptr)
{
    if (!omSetCof)
        omSetCof = OmSetupL::getInstance().OMS;
}
bool OmConnect::omConnectDevice(ConnectMode mode)
{
    Instrumentation::EnableLogger();
    if (!sttlogs) sttlogs = &nmainUI::statuslogs::getinstance();
    ipAddress = ConfigLocator::getInstance().settingconf->ipAddress;
    try
    {
        if (!device)
        {
            sttlogs->logInfo("Trying to Connnect to IP: " + ipAddress);
            device = DiscoverDevice();
            StartDevice();
        }    
            
        auto setup = Storage::CreateSetup();
        OpenView::ScanPlan::Create(setup);
        // OpenView::Configuration::Create(setup);
        //acquisition = IAcquisition::CreateEx(device);            
        //auto adjusted = OmConfigSetup::ConfigDeviceFromSetup(acquisition, device, setup);

        auto OmConfig = std::make_shared<OmConfigSetup>(acquisition, device, setup);
        OmConfig->ConfigDeviceSetting();
        sttlogs->logInfo ("Configured Setting for the device.");

        acquisition = IAcquisition::CreateEx(device);          
        auto setrate = acquisition->SetRate(omSetCof->Rate);
        acquisition->ApplyConfiguration();
        
        if (!datProcess) datProcess = std::make_shared<nDataProcess>(acquisition, device);    
        datProcess->Start();
    }
    catch (Instrumentation::ApplicationException& e) {
        omDisconnectDevice();
        sttlogs->logCritical("ApplicationException: " + std::string(e.what()));
        return false;
    }
    catch (Instrumentation::ArgumentException& e) {
        omDisconnectDevice();
        sttlogs->logCritical("ArgumentException: " + std::string(e.what()) + ", Argument: " + e.which());
        return false;
    }
    catch (Instrumentation::UnknownException& e) {
        omDisconnectDevice();
        sttlogs->logCritical("UnknownException: " + std::string(e.what()));
        return false;
    }
    catch (const std::exception& e) {
        omDisconnectDevice();
        sttlogs->logDebug( device->GetDriverConfiguration());        
        sttlogs->logCritical("Standard exception: " + std::string(e.what()));
        return false;
    }
    catch (...) {
        omDisconnectDevice();
        sttlogs->logCritical("Unexpected exception thrown.");
        return false;
    }
    return true;
}
void OmConnect::omDisconnectDevice()
{
    if (datProcess) datProcess->Stop(); datProcess = nullptr; 
}
shared_ptr<IDevice> OmConnect::DiscoverDevice()
{
    
    Duration timeout = ConfigLocator::getInstance().settingconf->timeout;
    auto discovery = IDeviceDiscovery::Create(ipAddress.c_str());
    DiscoverResult result = discovery->DiscoverFor(timeout);

    if (result.status != DiscoverResult::DeviceFound)
        throw std::runtime_error("# Error IP: No device were found.");
    sttlogs->logInfo("Found Device w Serial: " + result.device->GetInfo()->GetSerialNumber());
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
            sttlogs->logInfo("Package: " + wstostring(packages->GetFirmwarePackage(packageIndex)->GetName()));
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
        sttlogs->logInfo("Instrumentation Version: " + wstostring(version));
        OpenView::Libraries::UnLoad(handle);
        handle = OpenView::Libraries::LoadStorage();
        version = Olympus::FileManagement::Storage::GetLibraryVersion();
        sttlogs->logInfo("Storage Version: " + wstostring(version));
        OpenView::Libraries::UnLoad(handle);
    }
}

//void OmConnect::ConfigureDevice()
//{
//    if (!device) { throw std::exception("Device not connected."); }
//    std::cout << device->GetInfo();
//    std::shared_ptr<IBeamSet> beamSet;
//    auto ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();
//
//    // Detect the type of ultrasound (phased array or conventional)
//    auto digitizer = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
//    if (digitizer)
//    { beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetPhasedArray(L"BeamSet-PhasedArray", GenerateBeamFormations(digitizer->GetBeamSetFactory())); }
//    else
//    {
//        digitizer = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::Conventional);
//        beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetConventional(L"BeamSet-Conventional");
//    }
//    if (!digitizer)
//    {
//        throw std::exception("No valid ultrasound digitizer available.");
//    }
//    auto amplitudeSettings = beamSet->GetDigitizingSettings()->GetAmplitudeSettings();
//    amplitudeSettings->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);
//    for (size_t i = 0; i < beamSet->GetBeamCount(); ++i)
//    {
//        auto beam = beamSet->GetBeam(i);
//        beam->SetAscanStart(omSetCof->PA_AscanStart);
//        beam->SetAscanLength(omSetCof->PA_DigitizingLength);
//        cout << "Set Beam GateCollection Count: " << beam->GetGateCollection()->GetCount();
//        auto gate = beam->GetGateCollection()->GetGate(0);
//        gate->SetStart(0);
//        gate->SetLength(300);
//        gate->SetThreshold(15);
//        gate->InCycleData(true);
//    }
//
//    auto filterSettings = beamSet->GetDigitizingSettings()->GetFilterSettings();
//    auto digitalBandPassFilters = digitizer->GetDigitalBandPassFilterCollection();
//    auto digitalBandPassFilter = digitalBandPassFilters->GetDigitalBandPassFilter(5);
//    filterSettings->SetDigitalBandPassFilter(digitalBandPassFilter);
//
//    auto connector = digitizer->GetConnectorCollection()->GetPulseAndReceiveConnector();
//    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);
//}
//shared_ptr<IBeamFormationCollection> OmConnect::GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory)
//{
//    auto beamFormations = factory->CreateBeamFormationCollection();
//    for (size_t beam = 0; beam < 32; ++beam)
//    {
//        auto beamFormation = factory->CreateBeamFormation(32, 32);
//        auto pulserDelays = beamFormation->GetPulserDelayCollection();
//        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
//
//        for (size_t pulser = 0; pulser < 32; ++pulser)
//        {
//            pulserDelays->GetElementDelay(pulser)->SetElementId(pulser + 1);
//            pulserDelays->GetElementDelay(pulser)->SetDelay(((beam + pulser) * 2.5) + 500);
//        }
//
//        for (size_t receiver = 0; receiver < 32; ++receiver)
//        {
//            receiverDelays->GetElementDelay(receiver)->SetElementId(receiver + 1);
//            receiverDelays->GetElementDelay(receiver)->SetDelay(((beam + receiver) * 2.5) + 1000);
//        }
//
//        beamFormations->Add(beamFormation);
//    }
//
//    return beamFormations;
//}
