#include "OmConnect.h"
#include "OmCreateSetupSetting/LoadLibraries.h"
#include "SystemConfig/ConfigLocator.h"
using namespace Instrumentation;

OmConnect::OmConnect() : sttlogs(nullptr), acquisition(nullptr), datProcess(nullptr), OmConfig(nullptr)
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
            DiscoverDevice(device);
            sttlogs->logInfo("Connected to the IP Device: " + ipAddress);
        }    
                  
        if (!OmConfig)
        {
            // Init
            OmConfig = std::make_shared<OmConfigSetup>(device);
            acquisition =  OmConfig->ConfigDeviceSetting();
            if (!datProcess) datProcess = std::make_shared<nDataProcess>(acquisition, device);
            sttlogs->logInfo("Complete the initalization; Started.");
        }
        else
        {
            // Update
            if (!device || !acquisition) throw std::exception("Device or acquisition is null");
            OmConfig->ConfigUpdateSetting();
			sttlogs->logInfo("Updated the Acquisition.");
            datProcess->update();
        }       
        
    }
    catch (const std::exception& e) {
        omDisconnectDevice();     
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
void OmConnect::DiscoverDevice(IDevicePtr &device)
{
    auto wsToString = [](const std::wstring& wstr)->std::string { return std::string(wstr.begin(), wstr.end());};
    // Discover the Device
    Duration timeout = ConfigLocator::getInstance().settingconf->timeout;
    auto discovery = IDeviceDiscovery::Create(ipAddress.c_str());
    DiscoverResult result = discovery->DiscoverFor(timeout);

    if (result.status != DiscoverResult::DeviceFound)
        throw std::runtime_error("# Error IP: No device were found.");
    sttlogs->logInfo("Found Device w Serial: " + result.device->GetInfo()->GetSerialNumber());
    device = result.device;
    // Download and start the package
    wstring packageName(L"FocusPxPackage");
    shared_ptr<IFirmwarePackage> package;
    auto packages = IFirmwarePackageScanner::GetFirmwarePackageCollection();
    for (size_t packageIndex(0); packageIndex < packages->GetCount(); ++packageIndex)
    {
        if (packages->GetFirmwarePackage(packageIndex)->GetName().find(packageName) != string::npos)
        {
            package = packages->GetFirmwarePackage(packageIndex);
            sttlogs->logInfo("Package: " + wsToString(packages->GetFirmwarePackage(packageIndex)->GetName()));
        }
    }
    if (package == nullptr) throw std::exception("Could not find the firmware package");
    if (!device->HasPackage(package)) device->Download(package);
    device->Start(package);

    // Load Instrumentation and Storage Libraries    
    HINSTANCE handle = OpenView::Libraries::LoadInstrumentation();
    auto version = Instrumentation::GetLibraryVersionEx();
    sttlogs->logInfo("Instrumentation Version: " + wsToString(version));
    OpenView::Libraries::UnLoad(handle);
    handle = OpenView::Libraries::LoadStorage();
    version = Olympus::FileManagement::Storage::GetLibraryVersion();
    sttlogs->logInfo("Storage Version: " + wsToString(version));
    OpenView::Libraries::UnLoad(handle);
    device->GetConfiguration()->SetDeviceName(L"RopViewPAUTDevice");
}


