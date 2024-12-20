#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include "..\pch.h"
#include <instrumentation/Instrumentation.h>
#include "SystemConfig/ConfigLocator.h"


using namespace std;
using namespace Instrumentation;

//Auto start after initialization
class nDataProcess
{
    // internal properties
    std::mutex m_mtx2;
    bool exceptionFound = false;
    std::unique_ptr<std::atomic<bool>> m_running = std::make_unique<std::atomic<bool>>(false);
    std::future<void> m_future;
    ConfigLocator* configL;
    std::shared_ptr<Om_Settup_Config> omSetCof;
    bool isUpdate = false;
    // PAUT properties
    IAcquisitionPtr acquisition;
    IDevicePtr device;
    IFiringBeamSetPtr beamSet;

public:
    nDataProcess(IAcquisitionPtr gacquisition, IDevicePtr gdevice);
    ~nDataProcess();

    void FStop();
    void updateRealtimeProcess();

private:
    bool Start();
    void Run();
};

#endif