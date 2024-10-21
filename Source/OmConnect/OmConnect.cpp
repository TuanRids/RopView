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
    if (!sdk_logger)
    {

        /* Write & print logs
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs\\RopView.log", true);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        sdk_logger = std::make_shared<spdlog::logger>("RopView Logger", spdlog::sinks_init_list{ file_sink, console_sink });
        sdk_logger->set_level(spdlog::level::debug);  // Set log level
        spdlog::register_logger(spd_logger);

        sdk_logger->set_level(spdlog::level::debug);
        */
        sdk_logger = spdlog::basic_logger_mt("RopView Logger", "logs\\RopView.log", true);
        sdk_logger->set_level(spdlog::level::debug);

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
}
shared_ptr<IBeamFormationCollection> GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory);

void OmConnect::ConfigureDevice()
{
    if (!device)
    {
        throw std::exception("Device not connected.");
    }
    std::shared_ptr<IBeamSet> beamSet;

    // configure for phased array ultrasound
    auto ultrasoundConfiguration = device->GetConfiguration()->GetUltrasoundConfiguration();

    // detect the type of ultrasound is phased array or conventional
    auto digitizer = ultrasoundConfiguration->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);

    if (digitizer)
    {
        auto beamFormations = GenerateBeamFormations(digitizer->GetBeamSetFactory());
        beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetPhasedArray(L"BeamSet-PhasedArray", beamFormations);

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
        sttlogs->logNotify("Create conventional beam set: <" + wstostring(beamSet->GetName()) + "> using conventional / PhasedArray technology");

    auto filterSettings = beamSet->GetDigitizingSettings()->GetFilterSettings();
    auto digitalBandPassFilters = digitizer->GetDigitalBandPassFilterCollection();
    auto digitalBandPassFilter = digitalBandPassFilters->GetDigitalBandPassFilter(5);
    filterSettings->SetDigitalBandPassFilter(digitalBandPassFilter);

    auto connector = digitizer->GetConnectorCollection()->GetPulseAndReceiveConnector();
    ultrasoundConfiguration->GetFiringBeamSetCollection()->Add(beamSet, connector);

    acquisition = IAcquisition::CreateEx(device);
    acquisition->SetRate(config.Rate); // Hz
    acquisition->ApplyConfiguration();
}

// Shared buffer between threads
CircularBuffer<std::vector<int>> sharedBuffer(100);

void OmConnect::dataAcquisitionThread() {
    int ResIndex = 0;
    if (acquisition) { acquisition->Stop(); }
    acquisition->Start();
    int cycleIndexLimit = 1;
    while (true) {
        auto starttime = std::chrono::steady_clock::now();
        acquisition->Stop();
        acquisition->Start();
        for (size_t cycleIndex = 0; cycleIndex < 20; ++cycleIndex) {
            auto waitForDataResult = acquisition->WaitForDataEx();
            if (waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable) {
                std::cerr << "Error during data acquisition";
            }

            auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(0);
            std::vector<int> ascanVector(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());

            // std::lock_guard<std::mutex> lock(bufferMutex);
            sharedBuffer.push(ascanVector);

            /*auto beam = beamSet->GetBeam(0);
            beam->SetGainEx(beam->GetGain() + cycleIndex * 2);
            acquisition->ApplyConfiguration();*/
        }
        // duration milisec
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - starttime).count();
        sdk_logger->debug("CycleIndexLimit: {}, duration: {}, Throughput: {}", ++cycleIndexLimit, duration, acquisition->GetThroughput());
        sdk_logger->flush();
        writeData();
    }
}

void OmConnect::writeData()
{
    static int index = 0;
    while (sharedBuffer.size() > 0)
    {
        for (int i = 0; i < sharedBuffer.size(); ++i) {
            if (sharedBuffer.size() == 0) break;

            std::lock_guard<std::mutex> lock(bufferMutex);
            auto data = sharedBuffer.pop();


            std::ostringstream oss;
            int j;
            for (j = 0; j < data.size(); ++j) {
                if (data[j] == 0) { break; }
                oss << data[j] << " ";
                if (j % 11 == 10) {
                    oss << "";
                }
            }
            std::string dataStr = oss.str();
            sdk_logger->debug("id = {}; datasize {}/5000 ; data: {}", ++index, j, dataStr);
        }
    }
}

void OmConnect::newThread()
{
    if (!device)
    {
        test();
        sttlogs->logNotify("Trying to Connnect to IP: " + ipAddress);
        device = DiscoverDevice();
        StartDevice();
        ConfigureDevice();
    }
    //dataAcquisitionThread();
    /*std::thread acquisitionThread(&OmConnect::dataAcquisitionThread, this);
    acquisitionThread.detach();*/
    if (!datProcess) datProcess = std::make_shared<nDataProcess>(acquisition);
    datProcess->Start();
}

shared_ptr<IBeamFormationCollection> GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory)
{
    constexpr const size_t elementAperture = 16;
    constexpr const double DELAY_RESOLUTION = 2.5; //nanoseconds

    auto beamFormations = factory->CreateBeamFormationCollection();

    for (size_t beam(0); beam < 64; ++beam)
    {
        auto beamFormation = factory->CreateBeamFormation(elementAperture, elementAperture);
        auto pulserDelays = beamFormation->GetPulserDelayCollection();
        auto receiverDelays = beamFormation->GetReceiverDelayCollection();

        for (size_t pulser(0); pulser < elementAperture; ++pulser)
        {
            pulserDelays->GetElementDelay(pulser)->SetElementId(pulser + 1);
            pulserDelays->GetElementDelay(pulser)->SetDelay(((beam + pulser) * DELAY_RESOLUTION) + 500);
        }

        for (size_t receiver(0); receiver < elementAperture; ++receiver)
        {
            receiverDelays->GetElementDelay(receiver)->SetElementId(receiver + 1);
            receiverDelays->GetElementDelay(receiver)->SetDelay(((beam + receiver) * DELAY_RESOLUTION) + 1000);
        }

        beamFormations->Add(beamFormation);
    }

    return beamFormations;
}
