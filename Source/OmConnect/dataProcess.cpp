#include "dataProcess.h"
#include "MainUI/FacObsFrames/ObserverMgr.h"
#include "MainUI/statuslogs.h"
#include "OmConnect/OmConfigSetup.h"

std::shared_ptr<upFrame> obser = std::make_shared<upFrame>();

nDataProcess::nDataProcess(IAcquisitionPtr gacquisition, IDevicePtr gdevice)
    : acquisition(gacquisition), device (gdevice)
{
    if (!configL) configL = &ConfigLocator::getInstance();
    beamSet = device->GetConfiguration()->GetUltrasoundConfiguration()->GetFiringBeamSetCollection()->FindFiringBeamSet(L"LinearPhasedArray");
    if (!omSetCof) omSetCof = OmSetupL::getInstance().OMS;
    Start();
}


nDataProcess::~nDataProcess()
{
    Stop();
}

bool nDataProcess::Start()
{
    if (m_running) return true;
    m_running = true;
    m_future = std::async(std::launch::async, [this]() {
        std::string name = "Data Acquisition Thread";
        SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
        this->Run();
        });
	return true;
}

void nDataProcess::Stop()
{
    m_running = false;
    if (m_future.valid()) {
        acquisition->Stop();  
        m_future.wait();
    }

}

void nDataProcess::updateRealtimeProcess()
{
    std::lock_guard<std::mutex> lock(m_mtx2);
    isUpdate = true;
    m_future = {};
}

void nDataProcess::Run()
{    
    acquisition->Start();
    static auto readfps = &ReadStatus::getinstance();
    try
    {
        do
        {
            static double last_beamGain = 0; static int last_InternalDelay = -1; static double last_smoothFilter = -1;
            
            static QElapsedTimer fpsTimer;
            static int frameCount = 0;
            if (!fpsTimer.isValid()) { fpsTimer.start(); }
            if (fpsTimer.elapsed() >= 1000) {
                float avgEachFrameTime = fpsTimer.elapsed() / frameCount;
                readfps->set_readPAUT(avgEachFrameTime);

                fpsTimer.restart();
                frameCount = 0;
            }
            frameCount++;

            std::lock_guard<std::mutex> lock(m_mtx);                   
            auto waitForDataResult = acquisition->WaitForDataEx();
            if ( !acquisition || waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                nmainUI::statuslogs::getinstance().logCritical("~WaitForData Stopped (Reached the end of the cycles)");
                continue;
            }
            if (waitForDataResult.cycleData == nullptr) continue;   
            obser->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
            readfps->set_throughout(acquisition->GetThroughput());

            if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
            {
                auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                double crossingTime = !cscan->GetCrossingTime();
            }       
            
       
            if (isUpdate)
            {
                acquisition->ApplyConfiguration();
                isUpdate = false;
            }

        } while ((m_running));

    }
    catch (const exception& e)
    {
        cout << "ERROR : " << e.what() << "\n";
        m_running = false;
        acquisition->Stop();
        std::lock_guard<std::mutex> lock(m_mtx);
        nmainUI::statuslogs::getinstance().logCritical("Exception found: " + std::string(e.what()));
    }
    cout << "Stop " << "\n";
    acquisition->Stop();
    m_running = false;
}


