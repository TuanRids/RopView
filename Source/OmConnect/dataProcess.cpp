#include "dataProcess.h"


nDataProcess::nDataProcess(std::shared_ptr<IAcquisition> acquisition)
    : m_acquisition(acquisition), sharedBuffer(1000)
{
    sdk_logger = spdlog::get("RopView Logger");
}

nDataProcess::~nDataProcess()
{
    Stop();
}

void nDataProcess::Start()
{
    m_running = true;
    sdk_logger->info("New Thread Starting data acquisition...");
    m_future = std::async(std::launch::async, &nDataProcess::Run, this);
    QTimer* updateTimer = new QTimer();
    QObject::connect(updateTimer, &QTimer::timeout, [this]() {
        this->WriteData();
        });
    updateTimer->start(1000 / rate);
}

void nDataProcess::Stop()
{
    m_running = false;
    if (m_future.valid()) m_future.wait();
    if (m_future2.valid()) m_future2.wait();
}

void nDataProcess::Run()
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
                sdk_logger->error("Error during data acquisition.");
                m_acquisition->Stop();
                m_running = false;
                return;
            }

            {
                if (waitForDataResult.cycleData->GetAscanCollection()->GetCount() > 0)
                {
                    auto ascan = waitForDataResult.cycleData->GetAscanCollection()->GetAscan(0);
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
        } while ((m_running));
    }
    catch (const exception&)
    {
        exceptionFound = true;
    }

    m_acquisition->Stop();
    m_running = false;
}

void nDataProcess::WriteData()
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
            oss << data[j] << " ";
        }
        std::cout << oss.str() << std::endl;
        std::string dataStr = oss.str();
        sdk_logger->debug("ID: {}, DataSize: {}, Data: {}", ++index, data.size(), dataStr);
    }
}
