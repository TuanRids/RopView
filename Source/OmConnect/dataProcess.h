
#include <future>
#include <iostream>
#include <instrumentation/Instrumentation.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <sstream>
#include "CircularBuffer.h"
using namespace std;
using namespace Instrumentation;

class nDataProcess
{
    mutex m_mtx;
    shared_ptr<IAcquisition> m_acquisition;
    atomic<bool> m_running{ false };
    future<void> m_future, m_future2;
    std::shared_ptr<spdlog::logger> sdk_logger;
    CircularBuffer<std::vector<int>> sharedBuffer;

public:
    nDataProcess(shared_ptr<IAcquisition> acquisition)
        : m_acquisition(acquisition), sharedBuffer(1000)
    {
        sdk_logger = spdlog::get("RopView Logger");
    }

    ~nDataProcess()
    {
        Stop();
    }

    void Start()
    {
        m_running = true;
        sdk_logger->info("New Thread Starting data acquisition...");
        m_future = std::async(std::launch::async, &nDataProcess::Run, this);
        m_future2 = std::async(std::launch::async, &nDataProcess::WriteData, this);

    }

    void Stop()
    {
        m_running = false;
        if (m_future.valid())
        {
            m_future.wait();
        }
    }

private:
    void Run()
    {
        int ResIndex = 0;

        while (m_running)
        {
            m_acquisition->Start();
            std::this_thread::sleep_for(std::chrono::seconds(3));

            auto starttime = std::chrono::steady_clock::now();

            auto waitForDataResult = m_acquisition->WaitForDataEx();
            if (waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                sdk_logger->error("Error during data acquisition.");
                continue;
            }

            auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(0);
            std::vector<int> ascanVector(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());

            {
                std::lock_guard<std::mutex> lock(m_mtx);
                sharedBuffer.push(ascanVector);
            }

        m_acquisition->Stop();
        m_running = false;
    }
    void WriteData()
    {
        static int index = 0;
        while (m_running)
        {
            if (sharedBuffer.size() == 0) continue;
            std::lock_guard<std::mutex> lock(m_mtx);
            auto data = sharedBuffer.pop();
            std::ostringstream oss;
            for (int j = 0; j < data.size(); ++j)
            {
                // if (data[j] == 0) continue;
                oss << data[j] << " ";
            }
            std::cout << oss.str() << std::endl;
            std::string dataStr = oss.str();
            sdk_logger->debug("ID: {}, DataSize: {}/5000, Data: {}", ++index, data.size(), dataStr);
        }
    }
};
