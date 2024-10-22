#ifndef NOBSERVER_H
#define NOBSERVER_H
#include "../pch.h"
#include "PAUTFileReader/AscanDattype.hpp"
#include "MainUI/statuslogs.h"
#include "MainUI/mainwindow.h"
#include "event/ZoomableGraphicsView.h"
#include "OmConnect/BeamSet.h"
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
	void popSharedBuffer() { sharedBuffer->popBeamData(); }
protected:
	static bool isPanning;
	static curpt3d curpt;
	static AscanData scandat;
	static nmainUI::statuslogs* sttlogs;
	static std::unique_ptr<BeamSet> sharedBuffer;

	// TODO: Optimize this function later
	void UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene, std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor);
	std::vector<Color> CreateColorPalette();
};

class nSubject {
private:
	std::vector<std::shared_ptr<nObserver>> observers;
	bool isRealTime = false;
	QTimer* realTimeTimer;
public:
	nSubject() : realTimeTimer(new QTimer()) {
		QObject::connect(realTimeTimer, &QTimer::timeout, [this]() {
			this->notifyRealtime();
			});
	}
	~nSubject() { delete realTimeTimer;	}
	void addObserver(const std::shared_ptr<nObserver>& a_object) { observers.push_back(a_object); }
	void removeObserver(const std::shared_ptr<nObserver>& a_object) {
		observers.erase(std::remove(observers.begin(), observers.end(), a_object), observers.end());
	}
	// Notify all: nullptr
	void notify(nObserver* currentFrame) {
		for (const auto& object : observers) {
			if (isRealTime) return;
			if (currentFrame == nullptr) { object->update(); }
			if (object.get() && object.get() != currentFrame) {
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
		observers[0]->popSharedBuffer();
	}
};

class upFrame : public nObserver {
	QWidget* createFrame() override {return nullptr;}
	void update() override {}
	void updateRealTime() override {}
public:
	void upBuffer(int setId,int beamId, std::vector<int> data) { /* add data to buffer for Ascan & Sscan Realtime Render*/
		sharedBuffer->addBeamData(setId, beamId, data);
	}
};

#endif // NOBSERVER_H