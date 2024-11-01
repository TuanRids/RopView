#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include "..\pch.h"
#include <instrumentation/Instrumentation.h>
#include "SystemConfig/ConfigLocator.h"
using namespace std;
using namespace Instrumentation;

class nDataProcess
{
    std::mutex m_mtx;
    std::shared_ptr<IAcquisition> m_acquisition;
    std::atomic<bool> m_running{ false };
    std::future<void> m_future;

    bool exceptionFound = false;
    ConfigLocator* configL;
public:
    nDataProcess(std::shared_ptr<IAcquisition> acquisition);
    ~nDataProcess();

    bool Start();
    void Stop();

private:
    void Run();
};

#endif