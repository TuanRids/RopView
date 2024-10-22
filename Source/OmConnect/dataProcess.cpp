#include "dataProcess.h"
#include "MainUI/ObserverMgr.h"

std::shared_ptr<upFrame> obser = std::make_shared<upFrame>();
nDataProcess::nDataProcess(std::shared_ptr<IAcquisition> acquisition)
    : m_acquisition(acquisition)
{
    sdk_logger = spdlog::get("RopView Logger");
    if (!configL) configL = &ConfigLocator::getInstance();
}

nDataProcess::~nDataProcess()
{
    Stop();
}

void nDataProcess::Start()
{
    if (m_running) return;
    m_running = true;
    sdk_logger->info("New Thread Starting data acquisition...");
    m_future = std::async(std::launch::async, &nDataProcess::Run, this);
}


void nDataProcess::Stop()
{
    m_running = false;
    if (m_future.valid()) m_future.wait();

}

void nDataProcess::Run()
{
    bool exceptionFound(false);
    m_acquisition->Start();
    static size_t setIndex = 0;
    try
    {
        do
        {
            auto waitForDataResult = m_acquisition->WaitForDataEx();
            if (waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                sdk_logger->error("Error during data acquisition.");
                m_acquisition->Stop();
                m_running = false;
                return;
            }
            {
                if (waitForDataResult.cycleData->GetAscanCollection()->GetCount() > 0)
                {
                    for (int i(0); i < configL->omconf.beamLimit;++i)
                    {
                        auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(i);
                        auto xxx = std::vector<int>(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());
                        std::lock_guard<std::mutex> lock(m_mtx);
                        obser->upBuffer(setIndex,i,xxx);
                    }
                    //sharedBuffer.push(xxx);
                }
                if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
                {
                    auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                    double crossingTime = !cscan->GetCrossingTime();
                }
            }
            sdk_logger->debug("Throughput: {}", m_acquisition->GetThroughput()); 
            ++setIndex;
        } while ((m_running));
    }
    catch (const exception&)
    {
        exceptionFound = true;
    }

    m_acquisition->Stop();
    m_running = false;
}


