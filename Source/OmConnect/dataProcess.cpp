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
    acquisition->Stop();
    

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
            QElapsedTimer timer;  timer.start();

            auto waitForDataResult = acquisition->WaitForDataEx();

            if (acquisition->WaitForDataEx().status != IAcquisition::WaitForDataResultEx::DataAvailable)
            {
                acquisition->Stop();
                m_running = false;
                return;
            }

            if (waitForDataResult.cycleData == nullptr || waitForDataResult.cycleData->GetAscanCollection()->GetCount() == 0) continue;
            if (waitForDataResult.cycleData && waitForDataResult.cycleData->GetAscanCollection())
            {
                std::lock_guard<std::mutex> lock(m_mtx);                        
                obser->upAscanCollector(waitForDataResult.cycleData->GetAscanCollection());
                setthoughout->set(acquisition->GetThroughput());
            }           
                
            if (waitForDataResult.cycleData->GetCscanCollection()->GetCount() > 0)
            {
                auto cscan = waitForDataResult.cycleData->GetCscanCollection()->GetCscan(0);
                double crossingTime = !cscan->GetCrossingTime();
            }       

            recordReadingPAUT::getinstance().set_Fps(double(timer.nsecsElapsed() / 1e6f));
            //if (last_beamGain != omSetCof->phasing_gain)
            //{
            //    for (auto i = 0; i < beamSet->GetBeamSet()->GetBeamCount(); i++)
            //    {
            //        beamSet->GetBeamSet()->GetBeam(i)->SetGainEx(omSetCof->phasing_gain);
            //    }
            //    acquisition->ApplyConfiguration();
            //    last_beamGain = omSetCof->phasing_gain;
            //}
            //if (last_InternalDelay != omSetCof->PA_ElemInternalDelay) {
            //    for (auto beamIdx = 0; beamIdx < beamSet->GetBeamSet()->GetBeamCount(); beamIdx++)
            //    {
            //        auto ubeam = beamSet->GetBeamSet()->GetBeam(beamIdx);
            //        auto pulsers = ubeam->GetBeamFormation()->GetPulserDelayCollection();
            //        auto receivers = ubeam->GetBeamFormation()->GetReceiverDelayCollection();

            //        unsigned int VirAperture = omSetCof->EleFirst;
            //        auto NumberElem = omSetCof->EleQuantity; // wrong formula
            //        for (size_t elementIdx = 0; elementIdx < NumberElem; ++elementIdx) {
            //            pulsers->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            //            pulsers->GetElementDelay(elementIdx)->SetDelay((beamIdx + elementIdx) * omSetCof->PA_ElemInternalDelay + omSetCof->PA_elementDelay);
            //            receivers->GetElementDelay(elementIdx)->SetElementId(VirAperture);
            //            receivers->GetElementDelay(elementIdx)->SetDelay((beamIdx + elementIdx) * omSetCof->PA_ElemInternalDelay + omSetCof->PA_elementDelay * 2);
            //            VirAperture += omSetCof->EleStep;
            //        }
            //    }
            //    acquisition->ApplyConfiguration();
            //    last_InternalDelay = omSetCof->PA_ElemInternalDelay;
            //}


        } while ((m_running));

    }
    catch (const exception& e)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        nmainUI::statuslogs::getinstance().logCritical("Exception found: " + std::string(e.what()));
        exceptionFound = true;
        acquisition->Stop();
        m_running = false;
    }

    acquisition->Stop();
    m_running = false;
}


