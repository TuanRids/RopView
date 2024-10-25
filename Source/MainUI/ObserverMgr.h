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
#include <mutex>

using namespace std;
struct curpt3d { int x{ -1 }, y{ -1 }, z{ -1 };};

class nObserver {
public:
	nObserver();
	virtual ~nObserver() = default;

	virtual QWidget* createFrame() = 0;
	virtual void update() = 0;
	virtual void updateRealTime() = 0;

	void setScandat(const AscanData& dataa) { scandat = dataa; }
	void clearScandat() { scandat = AscanData(); }
	void popFront() {};
	void RealDatProcess();

	size_t bufferSize() { return _buffSize; }

protected:
	ConfigLocator ConfigL = ConfigLocator::getInstance();
	static bool isPanning;
	static curpt3d curpt;
	static AscanData scandat;
	static nmainUI::statuslogs* sttlogs;
	static deque<shared_ptr<IAscanCollection>> nAscanCollection;
	std::vector<Color> CreateColorPalette();
	static UIArtScan* ArtScan;
	std::mutex collectionMutex;
private:
	static size_t _buffSize;
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