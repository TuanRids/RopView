
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
        if (m_future2.valid())
		{
			m_future2.wait();
		}
    }

private:
    
    void Run()
    {
        int ResIndex = 0; size_t cycleId(0);
        bool exceptionFound(false);
        int beamNumber = 64;
        IAcquisition::WaitForDataResultEx dataResult;
        m_acquisition->Start();
        try
        {
            do
            {
                auto waitForDataResult = m_acquisition->WaitForDataEx();
                if (waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
                {
                    sdk_logger->error("Error during data acquisition."); m_acquisition->Stop(); m_running = false;
                    return;
                }
                {
                    if (waitForDataResult.cycleData->GetAscanCollection()->GetCount() > 0)
                    {
                        auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(0);
                        //std::span<const int> ascanSpan(ascan->GetData(), ascan->GetSampleQuantity());
                        auto xxx = std::vector<int>(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());
                        std::lock_guard<std::mutex> lock(m_mtx);
                        sharedBuffer.push(xxx);
                    }
                    if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
                    {
                        auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                        double crossingTime = !cscan->GetCrossingTime();
                    }
                }
                // sdk_logger->debug("CycleIndex: {}, Throughput: {}", ++ResIndex, m_acquisition->GetThroughput());
            } while ((m_running ));
        }
        catch (const exception&)
        {
            exceptionFound = true;
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
            sdk_logger->debug("ID: {}, DataSize: {}, Data: {}", ++index, data.size(), dataStr);
        }
        
    }
};

//==== Original vesion
/*#include <future>
#include <iostream>
#include <instrumentation/Instrumentation.h>

using namespace std;
using namespace Instrumentation;

class DataProcess
{
    mutex m_mtx;
    thread m_th;
    promise<size_t> m_prom;
    condition_variable m_cv;
    shared_ptr<IAcquisition> m_acquisition;
    atomic<bool> m_running{ false }, m_waitForAllData{ false };
    std::shared_ptr<spdlog::logger> sdk_logger;

public:
    DataProcess(shared_ptr<IAcquisition> acquisition)
        : m_acquisition(acquisition)
    { sdk_logger = spdlog::basic_logger_mt("RopView Logger", "RopView.log", false); };

    ~DataProcess()
    {
        m_waitForAllData = false;
        m_running = false;

        if (m_th.joinable())
            m_th.join();
    };

    void Start()
    {
        m_acquisition->Start();

        m_running = true;
        m_waitForAllData = true;

        m_prom = promise<size_t>();
        m_th.swap(thread(&DataProcess::Run, this));
    }

    void Stop(bool waitForAllData)
    {
        m_waitForAllData = waitForAllData;
        m_running = false;

        cout << endl;
        m_acquisition->Stop();

        if (m_th.joinable())
        {
            m_th.join();
            m_prom.get_future().get();
        }
    }

    void Wait(chrono::seconds duration)
    {
        unique_lock<mutex> lk(m_mtx);
        m_cv.wait_for(lk, duration, [&] { return !m_running.load(); });
    }
private:
    void Run()
    {
        size_t cycleId(0);
        bool exceptionFound(false);
        IAcquisition::WaitForDataResultEx dataResult;

        try
        {
            do
            {
                dataResult = m_acquisition->WaitForDataEx();
                if (dataResult.status == IAcquisition::WaitForDataResultEx::DataAvailable)
                {
                    auto cycleData = dataResult.cycleData;
                    cycleId = cycleData->GetCycleId();

                    cout << '\r' << "Acquiring cycle data: " << cycleId << flush;

                    if (cycleData->GetAscanCollection()->GetCount() > 0)
                    {
                        auto ascan = cycleData->GetAscanCollection()->GetAscan(0);
                        vector<int> ascanData(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());
                    }

                    if (cycleData->GetCscanCollection()->GetCount() > 0)
                    {
                        auto cscan = cycleData->GetCscanCollection()->GetCscan(0);
                        double crossingTime = !cscan->GetCrossingTime();
                    }
                }
            } while ((m_running || m_waitForAllData) && dataResult.status == IAcquisition::WaitForDataResultEx::DataAvailable);
        }
        catch (const exception&)
        {
            exceptionFound = true;
            m_prom.set_exception(current_exception());
        }

        if (!exceptionFound)
            m_prom.set_value(cycleId);

        m_running = false;
        m_cv.notify_all();
    }
};*/
