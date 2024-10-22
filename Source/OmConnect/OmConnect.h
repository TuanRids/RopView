#pragma once
#include "..\pch.h"
#include "IOmConnect.h"
#include "..\MainUI\statuslogs.h"

#include "dataProcess.h"
using namespace Instrumentation;
// vulkan & QT

using namespace std;

class OmConnect : public IOmConnect
{
public:
    OmConnect() :sttlogs(nullptr), acquisition(nullptr), beamSet(nullptr), datProcess(nullptr) {};
    ~OmConnect() { };
    bool omConnectDevice() override;
    void omDisconnectDevice() override;
private:
    // private methods
    shared_ptr<IDevice> DiscoverDevice() override;
    void StartDevice() override;
    void ConfigureDevice() override;
    void newThread();
    shared_ptr<IBeamFormationCollection> GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory);


    nmainUI::statuslogs* sttlogs;
    std::shared_ptr<IBeamSet> beamSet;
    std::shared_ptr<IAcquisition> acquisition;
    std::mutex bufferMutex;
    std::shared_ptr<spdlog::logger> sdk_logger;
    std::shared_ptr<nDataProcess> datProcess;
};


