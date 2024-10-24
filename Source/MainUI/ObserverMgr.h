#ifndef NOBSERVER_H
#define NOBSERVER_H
#include "../pch.h"
#include "PAUTFileReader/AscanDattype.hpp"
#include "MainUI/statuslogs.h"
#include "MainUI/mainwindow.h"
#include "event/ZoomableGraphicsView.h"
#include "OmConnect/BeamSet.h"
#include "Instrumentation/IAScanCollection.h"
#include "SystemConfig/ConfigLocator.h"
using namespace std;
struct curpt3d
{
	int x{ -1 }, y{ -1 }, z{ -1 };
	bool CheckIdx(int nx, int ny, int nz);
};


class nObserver {
public:
	virtual QWidget* createFrame() = 0;
	virtual void update() = 0;
	virtual void updateRealTime() = 0;
	virtual ~nObserver() = default;
	void setScandat(const AscanData& dataa) { scandat = dataa; }
	void clearScandat() { scandat = AscanData(); }
	void setSttlogs() { if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); } }
	void popFront() 
	{ while (!nAscanCollection.empty()) { nAscanCollection.pop_front(); } } //TODO Optimize this function
protected:
	ConfigLocator ConfigL = ConfigLocator::getInstance();
	static bool isPanning;
	static curpt3d curpt;
	static AscanData scandat;
	static nmainUI::statuslogs* sttlogs;
	static deque<shared_ptr<IAscanCollection>> nAscanCollection;
	// TODO: Optimize this function later
	void UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene, std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor);
	std::vector<Color> CreateColorPalette();
};

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
		~nSubject() { delete realTimeTimer; delete offlineTimer; 	}
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

		for (const auto& object : observers) {
			object->updateRealTime();
		}
		observers[0]->popFront();
	}
};

class upFrame : public nObserver {
	QWidget* createFrame() override {return nullptr;}
	void update() override {}
	void updateRealTime() override {}
public:
	void upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection) {
		nAscanCollection.push_back(_nAscanCollection);
	}
};

#endif // NOBSERVER_H