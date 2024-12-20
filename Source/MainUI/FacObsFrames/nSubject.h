#ifndef NSUBJECT_H
#define NSUBJECT_H

#include "../pch.h"
#include "statuslogs.h"

class nSubject {
public:
    nSubject() : offlineTimer(new QTimer()) {
        QObject::connect(offlineTimer, &QTimer::timeout, [this]() {
            this->notify();
            });
        if (!pautmgr) pautmgr = &PAUTManager::getInstance();
    }

    ~nSubject() {
        stopRealtimeUpdate();
        delete offlineTimer;
    }

    void addObserver(const std::shared_ptr<nObserver>& a_object) {
        observers.push_back(a_object);
    }

    void removeObserver(const std::shared_ptr<nObserver>& a_object) {
        observers.erase(std::remove(observers.begin(), observers.end(), a_object), observers.end());
    }

    void notify() {
        for (const auto& object : observers) {
            object->updateOffLine();
        }
    }

    void startRealtimeUpdate() {
        if (runRealtime) return; // Avoid duplicate threads
        runRealtime = true;
        
        realtimeThread = std::jthread([this](std::stop_token stopToken) {
            while (runRealtime && !stopToken.stop_requested()) {
                try {
                    pautmgr->waitAscan();
                    notifyRealtimeInternal();
                }
                catch (const std::exception& e) {
                    nmainUI::statuslogs::getinstance().logCritical(e.what());
                    runRealtime = false;
                }
            }
            });

        for (const auto& object : observers) {
            object->updateRealTime();
        }         
    }

    void stopRealtimeUpdate() {
        if (!runRealtime) return; // If already stopped, do nothing
        runRealtime = false;
        if (realtimeThread.joinable()) {
            realtimeThread.request_stop(); 
        }

        if (!observers.empty()) {
            pautmgr->clearAll();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    void startNotifyTimer(int intervalMs) {
        offlineTimer->start(intervalMs);
    }

    void stopNotifyTimer() {
        offlineTimer->stop();
        nmainUI::statuslogs::getinstance().logNotify(" Stop Offline Timer.");
    }
private:
    std::vector<std::shared_ptr<nObserver>> observers;
    std::atomic<bool> runRealtime{ false };
    std::jthread realtimeThread;
    QTimer* offlineTimer;
    std::mutex fpsmutex;
    PAUTManager* pautmgr;
    void notifyRealtimeInternal() {
        static FrameTimeTracker frameTracker;
        StartFrameTimer(frameTracker); // EndFrameTimer       

        if (pautmgr->isGLTexture()) {
            pautmgr->RealDatProcess();
        }
        else {
            pautmgr->processOnGPU();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (float avgFrameTime = EndFrameTimer(frameTracker); avgFrameTime > 0) {
            ReadStatus::getinstance().set_processData(avgFrameTime + 0.01);
        }
    }

};



#endif