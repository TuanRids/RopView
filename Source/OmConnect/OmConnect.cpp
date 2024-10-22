#include "OmConnect.h"
#include "LoadLibraries.h"

using namespace Instrumentation;
std::string wstostring(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}



bool OmConnect::omConnectDevice()
{
    if (!sdk_logger)
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs\\RopView.log", true);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        sdk_logger = std::make_shared<spdlog::logger>("RopView Logger", spdlog::sinks_init_list{ file_sink, console_sink });

        sdk_logger->set_level(spdlog::level::debug);

        spdlog::register_logger(sdk_logger);

        sdk_logger->flush_on(spdlog::level::info);

        sdk_logger->info("\n\t=================\n\tStarting OmConnect\n\t=================");
    }

    if (!sttlogs) sttlogs = &nmainUI::statuslogs::getinstance();
    ipAddress = SettingsManager::getInstance()->getSettings().ipAddress;
    try
    {
        // new thread
        newThread();
        return true;
    }
    catch (const std::exception& e)
    {
        sttlogs->logCritical(e.what());
        return false;
    }
}

void OmConnect::omDisconnectDevice()
{
    if (datProcess) datProcess->Stop();
}

shared_ptr<IDevice> OmConnect::DiscoverDevice()
{
    
    Duration timeout = SettingsManager::getInstance()->getSettings().timeout;
    auto discovery = IDeviceDiscovery::Create(ipAddress.c_str());
    DiscoverResult result = discovery->DiscoverFor(timeout);

    if (result.status != DiscoverResult::DeviceFound)
        throw std::runtime_error("# Error IP: No device were found.");
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

void OmConnect::ConfigureDevice()
{
    if (!device)
    {
        throw std::exception("Device not connected.");
    }

    std::shared_ptr<IBeamSet> beamSet;

    // Configure for phased array ultrasound
    auto ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();

    // Detect the type of ultrasound (phased array or conventional)
    auto digitizer = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);

    if (digitizer)
    {
        beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetPhasedArray(L"BeamSet-PhasedArray", GenerateBeamFormations(digitizer->GetBeamSetFactory()));
    }
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
        beam->SetAscanStart(config.ascanStart);
        beam->SetAscanLength(config.ascanLength);

        auto gate = beam->GetGateCollection()->GetGate(0);
        gate->SetStart(config.gateStart);
        gate->SetLength(config.gateLength);
        gate->SetThreshold(config.gateThreshold);
        gate->InCycleData(true);
    }

    auto filterSettings = beamSet->GetDigitizingSettings()->GetFilterSettings();
    auto digitalBandPassFilters = digitizer->GetDigitalBandPassFilterCollection();
    auto digitalBandPassFilter = digitalBandPassFilters->GetDigitalBandPassFilter(5);
    filterSettings->SetDigitalBandPassFilter(digitalBandPassFilter);

    auto connector = digitizer->GetConnectorCollection()->GetPulseAndReceiveConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);

    acquisition = IAcquisition::CreateEx(device);
    acquisition->SetRate(config.Rate); // Use Rate from config
    acquisition->ApplyConfiguration();
}




void OmConnect::newThread()
{
    if (!device)
    {
        sttlogs->logNotify("Trying to Connnect to IP: " + ipAddress);
        device = DiscoverDevice();
        StartDevice();
        ConfigureDevice();
    }
    if (!datProcess) datProcess = std::make_shared<nDataProcess>(acquisition);
    datProcess->Start();
}

shared_ptr<IBeamFormationCollection> OmConnect::GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory)
{
    auto beamFormations = factory->CreateBeamFormationCollection();

    for (size_t beam = 0; beam < config.beamLimit; ++beam)
    {
        auto beamFormation = factory->CreateBeamFormation(config.elementAperture, config.elementAperture);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        for (size_t pulser = 0; pulser < config.elementAperture; ++pulser)
        {
            pulserDelays->GetElementDelay(pulser)->SetElementId(pulser + 1);
            pulserDelays->GetElementDelay(pulser)->SetDelay(((beam + pulser) * config.delayResolution) + config.pulserBaseDelay);
        }

        for (size_t receiver = 0; receiver < config.elementAperture; ++receiver)
        {
            receiverDelays->GetElementDelay(receiver)->SetElementId(receiver + 1);
            receiverDelays->GetElementDelay(receiver)->SetDelay(((beam + receiver) * config.delayResolution) + config.receiverBaseDelay);
        }

        beamFormations->Add(beamFormation);
    }

    return beamFormations;
}

