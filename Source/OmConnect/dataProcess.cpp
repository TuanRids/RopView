#include "dataProcess.h"
#include "MainUI/ObserverMgr.h"

std::shared_ptr<upFrame> obser = std::make_shared<upFrame>();
nDataProcess::nDataProcess(std::shared_ptr<IAcquisition> acquisition)
    : m_acquisition(acquisition)
{
    if (!configL) configL = &ConfigLocator::getInstance();
}

nDataProcess::~nDataProcess()
{
    Stop();
}

void SetThreadName(const std::string& name) {
    HRESULT hr = SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
    if (FAILED(hr)) { }}

void nDataProcess::Start()
{
    if (m_running) return;
    m_running = true;
    // m_future = std::async(std::launch::async, &nDataProcess::Run, this);
    m_future = std::async(std::launch::async, [this]() {
        SetThreadName("Data Acquisition Thread");
        this->Run();
        });
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
                m_acquisition->Stop();
                m_running = false;
                return;
            }
            {
                if (waitForDataResult.cycleData->GetAscanCollection()->GetCount() > 0)
                {
                    for (int i(0); i < configL->omconf->beamLimit;++i)
                    {
                        std::lock_guard<std::mutex> lock(m_mtx);
                        obser->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
                    }
                }
                /*
                    if (sharedBuffer.size() == 0) continue;
                    std::lock_guard<std::mutex> lock(m_mtx);
                    auto data = sharedBuffer.pop();
                    std::ostringstream oss;
                    for (int j = 0; j < data.size(); ++j)
                    {
                        oss << data[j] << " ";
                    }
                    std::cout << oss.str() << std::endl;
                    std::string dataStr = oss.str();
                    sdk_logger->debug("ID: {}, DataSize: {}, Data: {}", ++index, data.size(), dataStr);
                */
                if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
                {
                    auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                    double crossingTime = !cscan->GetCrossingTime();
                }
            }
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


