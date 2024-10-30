#include "dataProcess.h"
#include "MainUI/FacObsFrames/ObserverMgr.h"
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
    m_acquisition->Stop();
    /*if (m_future.valid()) {
        if (m_future.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
            nmainUI::statuslogs::getinstance().logCritical("Forcing acquisition stop due to timeout.");
            m_acquisition->Stop();
        }
    }*/

}

void nDataProcess::Run()
{
    
    m_acquisition->Start();
    static auto setthoughout = &spdThoughout::getinstance();
    try
    {
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(4));
            auto waitForDataResult = m_acquisition->WaitForDataEx();
            if (m_acquisition->WaitForDataEx().status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                m_acquisition->Stop();
                m_running = false;
                return;
            }

            if (waitForDataResult.cycleData == nullptr || waitForDataResult.cycleData->GetAscanCollection()->GetCount() == 0) continue;
            if (waitForDataResult.cycleData && waitForDataResult.cycleData->GetAscanCollection())
            {
                std::lock_guard<std::mutex> lock(m_mtx);                        
                obser->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
                setthoughout->set(m_acquisition->GetThroughput());
            }           
                
            if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
            {
                auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                double crossingTime = !cscan->GetCrossingTime();
            }
        } while ((m_running));

    }
    catch (const exception& e)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        nmainUI::statuslogs::getinstance().logCritical("Exception found: " + std::string(e.what()));
        exceptionFound = true;
        m_acquisition->Stop();
        m_running = false;
    }

    m_acquisition->Stop();
    m_running = false;
}


