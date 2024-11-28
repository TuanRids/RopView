#ifndef NSUBJECT_H
#define NSUBJECT_H

#include "../pch.h"
#include "statuslogs.h"

class nSubject {
private:
    std::vector<std::shared_ptr<nObserver>> observers;
    std::atomic<bool> runRealtime{ false };
    std::thread realtimeThread;
    QTimer* offlineTimer;
    std::mutex fpsmutex;
    void notifyRealtimeInternal() {

        
        if (observers[0]->bufferSize() < 1) return;
        observers[0]->RealDatProcessGPU();
        
        static QElapsedTimer fpsTimer;
        static int frameCount = 0;
        if (!fpsTimer.isValid()) { fpsTimer.start(); }
        if (fpsTimer.elapsed() >= 1000) {
            float avgEachFrameTime = fpsTimer.elapsed() / frameCount;
            ReadStatus::getinstance().set_processData(avgEachFrameTime);

            fpsTimer.restart();
            frameCount = 0;
        }
        frameCount++;
    }

public:
    nSubject() : offlineTimer(new QTimer()) {
        QObject::connect(offlineTimer, &QTimer::timeout, [this]() {
            this->notify();
            });
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
        realtimeThread = std::thread([this]() {
            while (runRealtime) {
                try {
                    notifyRealtimeInternal();
                }
                catch (const std::exception& e) {
                    nmainUI::statuslogs::getinstance().logCritical(e.what());
                    stopRealtimeUpdate();
                    IOmConnect::ReleaseDevice();
                }
            }
            });
        for (const auto& object : observers) {
            object->updateRealTime();
        }         //  notify once time for switch the graphic viewport
    }

    void stopRealtimeUpdate() {
        runRealtime = false;
        if (realtimeThread.joinable()) {
            realtimeThread.join();
        }
        if (!observers.empty()) {
            observers[0]->clearBuffer();
            observers[0]->clearScandat();
        }
    }

    void startNotifyTimer(int intervalMs) {
        offlineTimer->start(intervalMs);
    }

    void stopNotifyTimer() {
        offlineTimer->stop();
        nmainUI::statuslogs::getinstance().logNotify(" Stop Offline Timer.");
    }
};
#endif