#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include "..\pch.h"
#include <instrumentation/Instrumentation.h>
#include "CircularBuffer.h"


using namespace std;
using namespace Instrumentation;

class nDataProcess
{
    std::mutex m_mtx;
    std::shared_ptr<IAcquisition> m_acquisition;
    std::atomic<bool> m_running{ false };
    std::future<void> m_future;
    std::shared_ptr<spdlog::logger> sdk_logger;
    CircularBuffer<std::vector<int>> sharedBuffer;
    unsigned int rate = 60;
public:
    nDataProcess(std::shared_ptr<IAcquisition> acquisition);
    ~nDataProcess();

    void Start();
    void Stop();

private:
    void Run();
};

#endif