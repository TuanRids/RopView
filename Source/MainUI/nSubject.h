#ifndef NSUBJECT_H
#define NSUBJECT_H

#include "../pch.h"
#include "statuslogs.h"

class nSubject {
private:
	std::vector<std::shared_ptr<nObserver>> observers;
	bool isRealTime = false;
	QTimer* realTimeTimer;
	QTimer* offlineTimer;
public:
	nSubject() : realTimeTimer(new QTimer()), offlineTimer(new QTimer()) {
		QObject::connect(realTimeTimer, &QTimer::timeout, [this]() {
			this->notifyRealtime();
			});
		QObject::connect(offlineTimer, &QTimer::timeout, [this]() {
			this->notify();
			});
	}
	~nSubject() { delete realTimeTimer; delete offlineTimer; }
	void addObserver(const std::shared_ptr<nObserver>& a_object) { observers.push_back(a_object); }
	void removeObserver(const std::shared_ptr<nObserver>& a_object) {
		observers.erase(std::remove(observers.begin(), observers.end(), a_object), observers.end());
	}
	// Notify all: nullptr
	void notify() {
		for (const auto& object : observers)
		{
			if (isRealTime) return;
			object->update();
		}
	}
	void startRealtimeUpdate(int intervalMs) {
		isRealTime = true;
		realTimeTimer->start(intervalMs);
	}
	void stopRealtimeUpdate() {
		isRealTime = false;
		realTimeTimer->stop();
	}
	void startNotifyTimer(int intervalMs) {
		offlineTimer->start(intervalMs);
	}
	void stopNotifyTimer() {
		offlineTimer->stop();
	}
	void notifyRealtime() {
		if (!isRealTime) return;
		try
		{	
			if (observers[0]->bufferSize() < 1) return;
			QElapsedTimer timer;
			timer.start();
			observers[0]->RealDatProcess();
			for (const auto& object : observers) {
				object->updateRealTime();
			}
			spdThoughout::getinstance().set_Fps(timer.elapsed());
		}
		catch (...)
		{ void(0); }
	}
};

#endif