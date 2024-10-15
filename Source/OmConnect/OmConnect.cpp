#include "OmConnect.h"



using namespace Instrumentation;
namespace omcn {
    bool OmConnect::omConnectDevice()
    {
        ipAddress = SettingsManager::getInstance()->getSettings().ipAddress;
        try
        {
            cout << "Connecting to device " << ipAddress << "..." << endl;
            device = DiscoverDevice();
            StartDevice();
            ConfigureDevice();
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl ;
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
                package = packages->GetFirmwarePackage(packageIndex);
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
        std::shared_ptr<IBeamSet> beamSet = beamSetFactory->CreateBeamSetConventional(L"BeamSet-UT");
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
        std::shared_ptr<IAcquisition> acquisition = IAcquisition::CreateEx(device);
        acquisition->ApplyConfiguration();

        // Acquire ultrasound data
        acquisition->Start();

        for (size_t cycleIndex = 0; cycleIndex < 25; ++cycleIndex)
        {
            auto waitForDataResult = acquisition->WaitForDataEx();
            if (waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
                throw std::exception("Error during data acquisition");

            auto cycleData = waitForDataResult.cycleData;

            std::cout << "Cycle Id: " << cycleData->GetCycleId() << std::endl;

            auto ascan = cycleData->GetAscanCollection()->GetAscan(0);
            std::vector<int> ascanData(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());

            // Increase gain on each cycle
            auto beam = beamSet->GetBeam(0);
            beam->SetGainEx(beam->GetGain() + cycleIndex);
            acquisition->ApplyConfiguration();
        }

        acquisition->Stop();
    }

    
}