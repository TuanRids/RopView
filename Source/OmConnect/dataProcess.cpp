#include "dataProcess.h"
#include "MainUI/ObserverMgr.h"
#include "MainUI/statuslogs.h"

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

bool nDataProcess::Start()
{
    if (m_running) return true;
    m_running = true;
    if (exceptionFound)  
    {
        exceptionFound = false; return false;
    }
    // m_future = std::async(std::launch::async, &nDataProcess::Run, this);
    m_future = std::async(std::launch::async, [this]() {
        SetThreadName("Data Acquisition Thread");
        this->Run();
        });
	return true;
}

void nDataProcess::Stop()
{
    m_running = false;
    if (m_future.valid()) m_future.wait();

}

void nDataProcess::Run()
{
    
    m_acquisition->Start();
    static size_t setIndex = 0;
    static auto setthoughout = &spdThoughout::getinstance();
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
                    std::lock_guard<std::mutex> lock(m_mtx);
                    obser->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
                    setthoughout->set(m_acquisition->GetThroughput());
                }

                /*auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(0);
                std::vector<int> ascanVector(ascan->GetData(), ascan->GetData() + ascan->GetSampleQuantity());
                std::ostringstream oss;
                for (int j = 0; j < ascanVector.size(); ++j)
                { oss << ascanVector[j] << " "; }
                std::cout << oss.str() << std::endl;*/

                
                if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
                {
                    auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                    double crossingTime = !cscan->GetCrossingTime();
                }
            }
            ++setIndex;
        } while ((m_running));

    }
    catch (const exception& e)
    {
        std::cerr << "Exception found" << e.what() << std::endl;
        exceptionFound = true;
        m_acquisition->Stop();
        m_running = false;
    }

    m_acquisition->Stop();
    m_running = false;
}


