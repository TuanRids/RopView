#ifndef OPV_SAVESETUPFROMDEVICE_H
#define OPV_SAVESETUPFROMDEVICE_H

#include "../pch.h"

using namespace std;
using namespace std::experimental::filesystem;
using namespace Instrumentation;
using namespace Olympus::Equipment;
using namespace Olympus::Inspection;
using namespace Olympus::FileManagement;
using namespace Olympus::FileManagement::Storage;

constexpr wchar_t DEVICE_NAME[] = L"Device A";
constexpr const size_t ELEMENT_QTY = 16;
constexpr const size_t FIRST_ELEMENT = 9;
constexpr const size_t BEAM_QTY = 10;
constexpr const NanoSecond ELEMENT_DELAY(102.5);

const path sampleFilesPath{ path{SOLUTION_DIR_PATH} / L"Sample Files" };

IDevicePtr GetConnectedDevice(string ipAddress);
IAcquisitionConfigurationPtr ConfigAcquisitionFromScanPlan(IDevicePtr device, ISetupPtr setup);

class OmSaveSetupFromDevice {
public:
    static int saveSetupfromDevice() {
        try
        {
            path inputFile = sampleFilesPath / L"ScanPlan.ovs";
            auto setupFile = OpenSetupFile(inputFile);
            auto setup = setupFile->GetSetup();

            string networkAdapter("192.168.0.1");
            auto device = GetConnectedDevice(networkAdapter);

            auto acqConfig = ConfigAcquisitionFromScanPlan(device, setup);

            // Configure any device settings...

            auto inspConfig = setup->ConvertAcquisitionConfiguration(acqConfig);
            setup->GetInspectionConfigurations()->Add(inspConfig);

            SaveSetupFile(L"..\\..\\Sample Files\\Setup.ovs", setup);
        }
        catch (exception const& ex)
        {
            cout << ex.what() << endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;

    }
    IDevicePtr GetConnectedDevice(string ipAddress)
    {
        DiscoverResult result;
        IDevicePtr device;
        Duration timeout(10000);

        auto discovery = IDeviceDiscovery::Create(ipAddress);
        result = discovery->DiscoverFor(timeout);

        if (result.status == DiscoverResult::DeviceFound)
        {
            device = result.device;

            auto packages = IFirmwarePackageScanner::GetFirmwarePackageCollection();
            auto package = packages->GetFirmwarePackage(0);

            if (!device->HasPackage(package))
                device->Download(package);

            device->Start(package);
            device->GetConfiguration()->SetDeviceName(DEVICE_NAME);
        }

        return device;
    }

    shared_ptr<IBeamFormationCollection> GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory)
    {
        auto beamFormations = factory->CreateBeamFormationCollection();
        for (size_t beamIdx(0); beamIdx < BEAM_QTY; ++beamIdx)
        {
            auto beamFormation = factory->CreateBeamFormation(ELEMENT_QTY, ELEMENT_QTY);
            auto pulserDelays = beamFormation->GetPulserDelayCollection();
            auto receiverDelays = beamFormation->GetReceiverDelayCollection();
            for (size_t elementIdx(0); elementIdx < ELEMENT_QTY; ++elementIdx)
            {
                pulserDelays->GetElementDelay(elementIdx)->SetElementId(FIRST_ELEMENT + elementIdx);
                pulserDelays->GetElementDelay(elementIdx)->SetDelay(ELEMENT_DELAY * elementIdx);

                receiverDelays->GetElementDelay(elementIdx)->SetElementId(FIRST_ELEMENT + elementIdx);
                receiverDelays->GetElementDelay(elementIdx)->SetDelay(ELEMENT_DELAY * elementIdx);
            }

            beamFormations->Add(beamFormation);
        }

        return beamFormations;
    }

    void AddConventionalFiringBeamSet(shared_ptr<IUltrasoundConfiguration> utConfig, const wstring& portName, const wstring& beamSetName)
    {
        shared_ptr<IBeamSet> beamSet;
        shared_ptr<IConnector> connector;

        if (portName == FocusPX::PA_PORT)
        {
            auto digitizer = utConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
            beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetConventional(beamSetName);
            connector = digitizer->GetConnectorCollection()->GetConnector(0);
        }
        else
        {
            auto digitizer = utConfig->GetDigitizerTechnology(UltrasoundTechnology::Conventional);
            beamSet = digitizer->GetBeamSetFactory()->CreateBeamSetConventional(beamSetName);

            if (portName == FocusPX::UT_P1_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(0);
            }
            else if (portName == FocusPX::UT_P2_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(1);
            }
            else if (portName == FocusPX::UT_P3_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(2);
            }
            else if (portName == FocusPX::UT_P4_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(3);
            }
            else if (portName == FocusPX::UT_P1R1_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(4);
            }
            else if (portName == FocusPX::UT_P2R2_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(5);
            }
            else if (portName == FocusPX::UT_P3R3_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(6);
            }
            else if (portName == FocusPX::UT_P4R4_PORT)
            {
                connector = digitizer->GetConnectorCollection()->GetConnector(7);
            }
        }

        utConfig->GetFiringBeamSetCollection()->Add(beamSet, connector);
    }

    void AddPhasedArrayFiringBeamSet(shared_ptr<IUltrasoundConfiguration> utConfig, const wstring& portName, const wstring& beamSetName)
    {
        auto digitizer = utConfig->GetDigitizerTechnology(UltrasoundTechnology::PhasedArray);
        auto factory = digitizer->GetBeamSetFactory();
        auto beamFormations = GenerateBeamFormations(factory);
        auto beamSet = factory->CreateBeamSetPhasedArray(beamSetName, beamFormations);

        if (portName == FocusPX::PA_PORT)
        {
            auto connector = digitizer->GetConnectorCollection()->GetConnector(0);
            utConfig->GetFiringBeamSetCollection()->Add(beamSet, connector);
        }
    }

    void CreateFiringBeamSetConventional(IInspectionMethodConventionalPtr inspMethod, shared_ptr<IUltrasoundConfiguration> utConfig)
    {
        auto peMethod = dynamic_pointer_cast<IInspectionMethodPulseEcho>(inspMethod);
        if (peMethod)
        {
            auto probeConv = dynamic_pointer_cast<IProbeConventional>(peMethod->GetProbe());
            if (probeConv)
            {
                wstring portName = probeConv->GetConnector()->GetConnection()->GetName();
                AddConventionalFiringBeamSet(utConfig, portName, peMethod->GetName());
            }
            else
            {
                auto probePa = dynamic_pointer_cast<IProbeRectangularArray>(peMethod->GetProbe());
                if (probePa)
                {
                    wstring portName = probePa->GetConnector()->GetConnection()->GetName();
                    AddConventionalFiringBeamSet(utConfig, portName, peMethod->GetName());
                }
                else
                    throw bad_cast{};
            }
        }
        else
        {
            auto pcMethod = dynamic_pointer_cast<IInspectionMethodPitchCatch>(inspMethod);
            if (pcMethod)
            {
                auto pProbe = dynamic_pointer_cast<IProbeConventional>(pcMethod->GetProbePulsing());
                auto rProbe = dynamic_pointer_cast<IProbeConventional>(pcMethod->GetProbeReceiving());
                if (pProbe && rProbe)
                {
                    //TODO
                }
                else
                    throw bad_cast{};
            }
            else
            {
                auto tofdMethod = dynamic_pointer_cast<IInspectionMethodTofd>(inspMethod);
                if (tofdMethod)
                {
                    auto pProbe = dynamic_pointer_cast<IProbeConventional>(tofdMethod->GetProbePulsing());
                    auto rProbe = dynamic_pointer_cast<IProbeConventional>(tofdMethod->GetProbeReceiving());
                    if (pProbe && rProbe)
                    {
                        //TODO
                    }
                    else
                        throw bad_cast{};
                }
                else
                    throw bad_cast{};
            }
        }
    }

    void CreateFiringBeamSetPhasedArray(IInspectionMethodPhasedArrayPtr paMethod, shared_ptr<IUltrasoundConfiguration> utConfig)
    {
        wstring portName;

        auto probePa = dynamic_pointer_cast<IProbeRectangularArray>(paMethod->GetProbe());
        if (probePa)
        {
            portName = probePa->GetConnector()->GetConnection()->GetName();
        }
        else
        {
            auto probeDualPa = dynamic_pointer_cast<IProbeDualRectangularArray>(paMethod->GetProbe());
            if (probeDualPa)
            {
                portName = probeDualPa->GetConnector()->GetConnection()->GetName();
            }
            else
                throw bad_cast{};
        }

        return AddPhasedArrayFiringBeamSet(utConfig, portName, paMethod->GetName());
    }

    IAcquisitionConfigurationPtr ConfigAcquisitionFromScanPlan(IDevicePtr device, ISetupPtr setup)
    {
        bool adjusted(false);
        auto utConfig = device->GetConfiguration()->GetUltrasoundConfiguration();

        auto inspMethods = setup->GetScanPlan()->GetInspectionMethodCollection();
        for (size_t methodIdx(0); methodIdx < inspMethods->GetCount(); ++methodIdx)
        {
            auto inspMethod = inspMethods->GetInspectionMethod(methodIdx);
            auto convMethod = dynamic_pointer_cast<IInspectionMethodConventional>(inspMethod);
            if (convMethod)
            {
                CreateFiringBeamSetConventional(convMethod, utConfig);
            }
            else
            {
                auto paMethod = dynamic_pointer_cast<IInspectionMethodPhasedArray>(inspMethod);
                if (paMethod)
                {
                    CreateFiringBeamSetPhasedArray(paMethod, utConfig);
                }
                else
                    throw bad_cast{};
            }
        }

        auto acq = IAcquisition::CreateEx(device);
        acq->ApplyConfiguration();

        return acq->GetConfiguration();
    }


};
#endif