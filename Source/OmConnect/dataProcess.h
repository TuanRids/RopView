#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include "..\pch.h"
#include <instrumentation/Instrumentation.h>
#include "SystemConfig/ConfigLocator.h"


using namespace std;
using namespace Instrumentation;

class nDataProcess
{
    // internal properties
    std::mutex m_mtx;
    bool exceptionFound = false;
    std::atomic<bool> m_running{ false };
    std::future<void> m_future;
    ConfigLocator* configL;
    std::shared_ptr<Om_Settup_Config> omSetCof;

    // PAUT properties
    IAcquisitionPtr acquisition;
    IDevicePtr device;
    IFiringBeamSetPtr beamSet;

public:
    nDataProcess(std::shared_ptr<IAcquisition> gacquisition, IDevicePtr gdevice);
    ~nDataProcess();

    bool Start();
    void Stop();

private:
    void Run();
};

#endif