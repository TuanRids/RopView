#pragma once
#include "../pch.h"
#include "IOmConnect.h"
#include "MainUI/statuslogs.h"
#include "SystemConfig/ConfigLocator.h"
#include "dataProcess.h"
#include "OmConfigSetup.h"
using namespace Instrumentation;
using namespace std;

class OmConnect : public IOmConnect
{
public:
    OmConnect();
    ~OmConnect() { };

    bool omConnectDevice(ConnectMode mode) override;
    void omDisconnectDevice() override;
private:
    // private methods
    shared_ptr<IDevice> DiscoverDevice() override;
    void StartDevice() override;
    void ConfigureDevice() override;
    shared_ptr<IBeamFormationCollection> GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory);


    ConfigLocator* configL;
    nmainUI::statuslogs* sttlogs;
    std::shared_ptr<IBeamSet> beamSet;
    std::shared_ptr<IAcquisition> acquisition;
    std::mutex bufferMutex;
    std::shared_ptr<nDataProcess> datProcess;
};


