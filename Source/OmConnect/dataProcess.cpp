#include "dataProcess.h"
#include "MainUI/FacObsFrames/ObserverMgr.h"
#include "MainUI/statuslogs.h"
#include "OmConnect/OmConfigSetup.h"

PAUTManager* PAUTmgr = &PAUTManager::getInstance();

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
   FStop();
}

bool nDataProcess::Start()
{
    if (*m_running) return true;
    *m_running = true;
    m_future = std::async(std::launch::async, [this]() {
        std::string name = "Data Acquisition Thread";
        SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
        this->Run();
        });
	return true;
}

void nDataProcess::FStop()
{
    if (!m_running) return;
    *m_running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (m_future.valid()) {
        if (acquisition )
        {
            if (acquisition->GetState() != Instrumentation::IAcquisition::State::Stopped) acquisition->Stop();
            acquisition.reset();
            acquisition = nullptr;
        }
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
    FrameTimeTracker frameTracker;
    try
    {
        do
        {
            static double last_beamGain = 0; static int last_InternalDelay = -1; static double last_smoothFilter = -1;
            StartFrameTimer(frameTracker); // EndFrameTimer

            auto waitForDataResult = acquisition->WaitForDataEx();
            if ( !acquisition || waitForDataResult.status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                nmainUI::statuslogs::getinstance().logCritical("~WaitForData Stopped (Reached the end of the cycles)");
                continue;
            }
            if (waitForDataResult.cycleData == nullptr) continue;   
            {
                // use unique_lock for writing the data
                // ::unique_lock<std::shared_mutex> lock(PAUTmgr->getCollectionMutex());
                // No need to mutex, blockingqueue has its own mutex
                PAUTmgr->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
            }
            readfps->set_throughout(acquisition->GetThroughput());

            if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
            {
                auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                double crossingTime = !cscan->GetCrossingTime();
            } 

            if (float avgFrameTime = EndFrameTimer(frameTracker); avgFrameTime > 0) { readfps->set_readPAUT(avgFrameTime); }
        } while ((*m_running));

    }
    catch (const exception& e)
    {
        cout << "ERROR : " << e.what() << "\n";
        *m_running = false;
        if (acquisition)
        {
            acquisition->Stop(); acquisition.reset();
            acquisition = nullptr;
        }
        nmainUI::statuslogs::getinstance().logCritical("Exception found: " + std::string(e.what()));
    }
    cout << ">>Stop PAUT RUNNING" << "\n";
    if (acquisition) acquisition->Stop();  acquisition.reset(); acquisition = nullptr;
    *m_running = false;
}


