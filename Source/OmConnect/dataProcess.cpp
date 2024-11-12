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
    acquisition->Stop();  
}

void nDataProcess::update()
{
    std::lock_guard<std::mutex> lock(m_mtx2);
    isUpdate = true;
}

void nDataProcess::Run()
{    
    acquisition->Start();
    static auto setthoughout = &recordReadingPAUT::getinstance();
    try
    {
        do
        {
            static double last_beamGain = 0; static int last_InternalDelay = -1; static double last_smoothFilter = -1;
            static std::deque<float> timeLapses;
            static const int maxSamples = 10;
            static QElapsedTimer timer;  

            std::lock_guard<std::mutex> lock(m_mtx);                       
            
            auto waitForDataResult = acquisition->WaitForDataEx();
            if ( !acquisition || acquisition->WaitForDataEx().status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                nmainUI::statuslogs::getinstance().logCritical("~WaitForData Stopped (Reached the end of the cycles)");
                continue;
            }
            if (waitForDataResult.cycleData == nullptr) continue;   
            obser->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
            setthoughout->set(acquisition->GetThroughput());             

            if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
            {
                auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                double crossingTime = !cscan->GetCrossingTime();
            }       

            recordReadingPAUT::getinstance().set_Fps(double(timer.nsecsElapsed() / 1e6f));     
            if (isUpdate) 
            {
                acquisition->ApplyConfiguration();
                isUpdate = false;
            }
            timer.start();
            //this_thread::sleep_for(std::chrono::milliseconds(50));

        } while ((m_running));

    }
    catch (const exception& e)
    {
        cout << "STOPPPP\n";
        m_running = false;
        acquisition->Stop();
        std::lock_guard<std::mutex> lock(m_mtx);
        nmainUI::statuslogs::getinstance().logCritical("Exception found: " + std::string(e.what()));
    }
    acquisition->Stop();
    m_running = false;
}


