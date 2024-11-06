#ifndef __OM_CONNECT_H__
#define __OM_CONNECT_H__
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

    nmainUI::statuslogs* sttlogs;
    std::shared_ptr<IAcquisition> acquisition;
    std::mutex bufferMutex;
    std::shared_ptr<nDataProcess> datProcess;
    std::shared_ptr<Om_Settup_Config> omSetCof;
};


/*
std::shared_ptr<IBeamSet> beamSet;
void ConfigureDevice() override;
shared_ptr<IBeamFormationCollection> GenerateBeamFormations(shared_ptr<IBeamSetFactory> factory);
*/
#endif