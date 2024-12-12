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
            // Atomic flag and condition variable
            std::atomic<bool> isDeviceDiscovered{ false };
            std::mutex mtx;
            std::condition_variable cv;

            // Create progress dialog
            QProgressDialog progress("Discovering the device...", "Cancel", 0, 100);
            progress.setWindowModality(Qt::WindowModal);
            progress.setCancelButton(nullptr); // Disable cancel button
            progress.setMinimumDuration(0);    // Show immediately

            // Launch the discovery thread
            std::thread discoverThread([&]() {
                isDeviceDiscovered = DiscoverDevice(device); // Attempt to discover the device
                std::unique_lock<std::mutex> lock(mtx);
                cv.notify_one(); // Notify the main thread that discovery is complete
                });

            // Simulate progress bar for 3 seconds
            for (int i = 0; i <= 100; ++i) {
                progress.setValue(i);
                if (i == 15) {
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        progress.setLabelText("Create Device, Active Firmware,...");
                        if (!cv.wait_for(lock, std::chrono::seconds(3), [&]() { return isDeviceDiscovered.load(); })) {
                            cv.wait(lock, [&]() { return isDeviceDiscovered.load(); });
                        }
                    }
                }
                if (i < 35) {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(500000));
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    i += 4;
                }
                
            }
            // Ensure the thread is joined
            discoverThread.join();

            // Check the discovery status
            if (isDeviceDiscovered && device) {
                sttlogs->logInfo("Connected to the IP Device: " + ipAddress);
            }
            else {
                sttlogs->logCritical("Failed to connect to the IP Device: " + ipAddress);
            }
        }

        if (!OmConfig || !acquisition)
        {
            // Init
            OmConfig = std::make_shared<OmConfigSetup>(device);
            acquisition = OmConfig->ConfigDeviceSetting();
            if (!datProcess) datProcess = std::make_shared<nDataProcess>(acquisition, device);
            sttlogs->logInfo("Complete the initalization; Started.");
        }
        else
        {
            // Update
            if (!device || !acquisition) throw std::exception("Device or acquisition is null");
            if (datProcess)
            {
                datProcess->FStop(); datProcess = nullptr; acquisition.reset(); acquisition = nullptr;
                OmConfig->removeAcquisition();
                sttlogs->logInfo("Stopped the Acquisition.");
            }
            device->ResetConfiguration();
            acquisition = OmConfig->ConfigDeviceSetting();
            datProcess = std::make_shared<nDataProcess>(acquisition, device);
            sttlogs->logInfo("Updated the Acquisition.");
            IOmConnect::isUpdate = true;
        }

    }
    catch (const std::exception& e) {
        if (datProcess) datProcess->FStop();
        sttlogs->logCritical("Standard exception: " + std::string(e.what()));
        return false;
    }
    catch (...) {
        datProcess->FStop();
        sttlogs->logCritical("Unexpected exception thrown.");
        return false;
    }
    return true;
}
void OmConnect::omDisconnectDevice()
{
    if (datProcess) datProcess->FStop(); datProcess = nullptr;
}
bool OmConnect::DiscoverDevice(IDevicePtr& device)
{
    auto wsToString = [](const std::wstring& wstr)->std::string { return std::string(wstr.begin(), wstr.end()); };
    // Discover the Device
    Duration timeout = ConfigLocator::getInstance().settingconf->timeout;
    auto discovery = IDeviceDiscovery::Create(ipAddress.c_str());
    DiscoverResult result = discovery->DiscoverFor(timeout);

    if (result.status != DiscoverResult::DeviceFound)
        return false;
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
    return true;
}


