#include "OmConnect.h"

#include "LoadLibraries.h"

using namespace Instrumentation;
std::string wstostring(const std::wstring& wstr) {
	return std::string(wstr.begin(), wstr.end());
}
int OmConnect::test() {
    try
    {
        {
            HINSTANCE handle = OpenView::Libraries::LoadInstrumentation();
            auto version = Instrumentation::GetLibraryVersionEx();
            sttlogs->logNotify("Instrumentation Version: " + wstostring(version));
            OpenView::Libraries::UnLoad(handle);
        }

        {
            HINSTANCE handle = OpenView::Libraries::LoadStorage();
            std::wstring version = Olympus::FileManagement::Storage::GetLibraryVersion();
            sttlogs->logNotify("Storage Version: " + wstostring(version));
            OpenView::Libraries::UnLoad(handle);
        }
    }
    catch (const std::exception& ex)
    {
        sttlogs->logCritical(ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool OmConnect::omConnectDevice()
{
    if (!sttlogs) sttlogs = &nmainUI::statuslogs::getinstance(); 
    ipAddress = SettingsManager::getInstance()->getSettings().ipAddress;
    try
    {
        if (!device)
        {
            test();
            sttlogs->logNotify("Trying to Connnect to IP: " + ipAddress);
            device = DiscoverDevice();
            StartDevice();
            ConfigureDevice();
        }
        acquisitionStart();
        return true;
    }
    catch (const std::exception& e)
    {
        sttlogs->logCritical(e.what() );
        return false;
    }
}

shared_ptr<IDevice> OmConnect::DiscoverDevice()
{
    Duration timeout = SettingsManager::getInstance()->getSettings().timeout;
    auto discovery = IDeviceDiscovery::Create(ipAddress.c_str());
    DiscoverResult result = discovery->DiscoverFor(timeout);

    if (result.status != DiscoverResult::DeviceFound)
        throw std::runtime_error("# Error IP: No device were found." );
    sttlogs->logNotify("Found Device w Serial: " + result.device->GetInfo()->GetSerialNumber());
    return result.device;
}

void OmConnect::StartDevice()
{
    // Select the latest version of firmware packages.
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
}

void OmConnect::ConfigureDevice()
{
    if (!device)
    {throw std::exception("Device not connected.");}

    // Create ultrasound configuration using one conventional beam set on P1/R1 connector
    std::shared_ptr<IUltrasoundConfiguration> ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();

    std::shared_ptr<IBeamSetFactory> beamSetFactory = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::Conventional)->GetBeamSetFactory();
    beamSet = beamSetFactory->CreateBeamSetConventional(L"BeamSet-UT");
    sttlogs->logNotify("Create conventional beam set: <" + wstostring(beamSet->GetName()) + "> using conventional / PhasedArray technology");
    std::shared_ptr<IAmplitudeSettings> amplitudeSettings = beamSet->GetDigitizingSettings()->GetAmplitudeSettings();
    amplitudeSettings->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::TwelveBits);

    // Set filter from capabilities list. 
    std::shared_ptr<IFilterSettings> filterSettings = beamSet->GetDigitizingSettings()->GetFilterSettings();
    std::shared_ptr<IDigitalBandPassFilterCollection> digitalBandPassFilters = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::Conventional)->GetDigitalBandPassFilterCollection();
    std::shared_ptr<IDigitalBandPassFilter> digitalBandPassFilter = digitalBandPassFilters->GetDigitalBandPassFilter(5);
    beamSet->GetDigitizingSettings()->GetFilterSettings()->SetDigitalBandPassFilter(digitalBandPassFilter);

    // Bind beam set with a compatible connector.
    std::shared_ptr<IConnector> connector = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::Conventional)->GetConnectorCollection()->GetPulseAndReceiveConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);

    // Apply the configuration changes
    acquisition = IAcquisition::CreateEx(device);
    acquisition->ApplyConfiguration();
    
    

}

void OmConnect::acquisitionStart()
{
    // Acquire ultrasound data
    acquisition->Start();
    auto starttime = std::chrono::system_clock::now();

    std::string z = "";
    for (size_t cycleIndex = 0; cycleIndex < 2; ++cycleIndex)
    {
        z = "";
        auto waitForDataResult = acquisition->WaitForDataEx();
        if (waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
            throw std::exception("Error during data acquisition");

        auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(0);
        std::span<const int> ascanData(ascan->GetData(), ascan->GetSampleQuantity());

        for (int i = 0 ; i < ascanData.size(); i++)
        {
            z += "-" + std::to_string(ascanData[i]);
            if (i%11 == 0) z += "\n";
        }
        /*auto beam = beamSet->GetBeam(0);
        beam->SetGainEx(beam->GetGain() + cycleIndex);
        acquisition->ApplyConfiguration();*/
    }
    // duration ms 

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - starttime);
    sttlogs->logNotify(z);
	sttlogs->logNotify("Duration: " + std::to_string(duration.count()));
    //acquisition->Stop();

}

void OmConnect::acquisitionStop()
{
    try
    {
        acquisition->Stop();
    }
	catch (const std::exception& e)
    { void(0); }
}

    