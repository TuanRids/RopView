#ifndef NOBSERVER_H
#define NOBSERVER_H

#include "vector"
#include "FactoryMgr.h"
class nObserver {
public:
	virtual void update() = 0;
	virtual void updateRealTime() = 0;
	virtual ~nObserver() = default;
};

class nSubject {
private:
	std::vector<std::shared_ptr<nObserver>> observers; // Static observers
	bool isRealTime = false;
	QTimer* realTimeTimer;
public:
	nSubject() : realTimeTimer(new QTimer()) {
		QObject::connect(realTimeTimer, &QTimer::timeout, [this]() {
			this->notifyRealtime();
			});
	}
	~nSubject() {
		delete realTimeTimer;
	}
	void addObserver(const std::shared_ptr<nObserver>& a_object) { observers.push_back(a_object); }
	void removeObserver(const std::shared_ptr<nObserver>& a_object) {
		observers.erase(std::remove(observers.begin(), observers.end(), a_object), observers.end());
	}
	// Notify all: nullptr
	void notify(nFrame* currentFrame) {
		for (const auto& object : observers) {
			if (isRealTime) return;
			if (currentFrame == nullptr) { object->update(); }
			nFrame* frameObserver = dynamic_cast<nFrame*>(object.get()); // TODO add log/try catch if dynamic cast fails
			if (frameObserver && frameObserver != currentFrame) {
				object->update();
			}
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
	void notifyRealtime() {
		if (!isRealTime) return;
		for (const auto& object : observers) {
			object->updateRealTime();
		}
	}
};

#endif // NOBSERVER_H