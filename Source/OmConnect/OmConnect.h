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
    bool DiscoverDevice(IDevicePtr& device) override;

    nmainUI::statuslogs* sttlogs;
    IAcquisitionPtr acquisition;
    std::mutex bufferMutex;
    std::shared_ptr<nDataProcess> datProcess;
    std::shared_ptr<Om_Settup_Config> omSetCof;
    std::shared_ptr<OmConfigSetup> OmConfig;
};

#endif