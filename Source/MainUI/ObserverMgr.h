#ifndef NOBSERVER_H
#define NOBSERVER_H

#include <vector>
#include "FactoryMgr.h"
#include <memory>
#include <functional>
#include <QTimer>
#include <string>

struct Color;
class AscanData;
class ZoomableGraphicsView;

struct curpt3d
{
    int x{ -1 }, y{ -1 }, z{ -1 };
    bool CheckIdx(int nx, int ny, int nz);
};

class nFrame {
public:
    virtual ~nFrame() = default;
	void setScandat(const AscanData& dataa);
	void clearScandat();
    void setSttlogs() { if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); } }
    void UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene,
        std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor);

protected:
    static bool isPanning;
    static curpt3d curpt;
    static AscanData scandat;
    static nmainUI::statuslogs* sttlogs;
	std::vector<Color> CreateColorPalette();
};


class nObserver {
public: 
    virtual void update() = 0;
	virtual ~nObserver() = default;
};

// Free to create and call notify anywhere
class nSubject {
private:
	std::vector<std::shared_ptr<nObserver>> obsOffline; // Static obsOffline
	std::vector<std::shared_ptr<nObserver>> obsRealTime; // Static obsOffline
	bool isRealtime = false;  
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

	void addOfflineObserver(const std::shared_ptr<nObserver>& a_object) {
		obsOffline.push_back(a_object);
	}

	void addRealtimeObserver(const std::shared_ptr<nObserver>& a_object) {
		obsRealTime.push_back(a_object);
	}
	void notify(nFrame* currentFrame) {
		if (isRealtime) return;
		for (const auto& object : obsOffline) {
			if (currentFrame == nullptr) 
			{ object->update();}
			nFrame* frameObserver = dynamic_cast<nFrame*>(object.get());
			if (frameObserver && frameObserver != currentFrame) {
				object->update();
			}
		}
	}
	void startRealtimeUpdate(int intervalMs) {
		isRealtime = true;
		realTimeTimer->start(intervalMs);  
	}
	void stopRealtimeUpdate() {
		isRealtime = false;
		realTimeTimer->stop();  
	}
	void notifyRealtime() {
		if (!isRealtime) return;  

		for (const auto& object : obsRealTime) {
			object->update();  
		}
	}
};

#endif // NOBSERVER_H
