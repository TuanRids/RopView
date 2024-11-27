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

struct VertexData {
	QVector2D position;  
	QVector3D color;    
};

using namespace std;
struct curpt3d { int x{ -1 }, y{ -1 }, z{ -1 };};

class nObserver {
public:
	nObserver();
	virtual ~nObserver() = default;

	virtual QWidget* createFrame() = 0;
	virtual void updateOffLine() = 0;
	virtual void updateRealTime() = 0;

	void setScandat(const AscanData& dataa) { scandat = dataa; }
	void clearScandat() { scandat = AscanData(); ArtScan->resetAll(); }
	void clearBuffer() { nAscanCollection.clear(); }
	void popFront() {};

	void RealDatProcess();
	void RealDatProcessGPU();
	size_t bufferSize() { return nAscanCollection.size(); }
	void upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection) {
		std::lock_guard<std::mutex> lock(collectionMutex);
		if (!_nAscanCollection) return;
		nAscanCollection.push_back(_nAscanCollection);		
	}

protected:
	static QVector<VertexData> vertice_sview; // temporary


	ConfigLocator* ConfigL = &ConfigLocator::getInstance();
	OmSetupL oms = OmSetupL::getInstance();
	static bool isPanning;
	static curpt3d curpt;
	static AscanData scandat;
	static nmainUI::statuslogs* sttlogs;
	static deque<shared_ptr<IAscanCollection>> nAscanCollection;
	std::vector<Color> CreateColorPalette(int gainFactor );
	static UIArtScan* ArtScan;
	std::mutex collectionMutex;
private:
	static size_t _buffSize;
};



class upFrame : public nObserver {
	QWidget* createFrame() override {return nullptr;}
	void updateOffLine() override {}
	void updateRealTime() override {}
	std::mutex upMutex;
public:

};

#endif // NOBSERVER_H