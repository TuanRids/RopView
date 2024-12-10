#ifndef NOBSERVER_H
#define NOBSERVER_H
#include "../pch.h"
#include "PAUTFileReader/AscanDattype.hpp"
#include "MainUI/statuslogs.h"
#include "MainUI/mainwindow.h"
#include "event/ZoomableGraphicsView.h"
#include "OmConnect/BeamSet.h"
#include "Instrumentation/IAScanCollection.h"
#include <mutex>
#include "ObserverStruct.hpp"


using namespace std;
class nObserver {
public:
	nObserver();
	virtual ~nObserver() = default;

	virtual QWidget* createFrame() = 0;
	virtual void updateOffLine() = 0;
	virtual void updateRealTime() = 0;

	//process
	void RealDatProcess();
	void processOnGPU(); // use childClass to process by OpenGL4_3 for avoiding multi inhertitance

	//setter
	void setScandat(const AscanData& dataa) { prosdt.scandat = dataa; }
	void clearAll() { prosdt.scandat = AscanData(); prosdt.ArtScan->resetAll(); prosdt.nAscanCollection.clear(); }
	void upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection);
	
	// getter & check
	bool isGLTexture() { return prosdt.nIsGlTexture.load(); }
	std::shared_mutex &getCollectionMutex() { return collectionMutex; }
	size_t bufferSize() { return prosdt.nAscanCollection.size(); }

protected:
	void updateParameters(std::shared_ptr<IAscanCollection>& RawAsanDat);
	static ProcessingContext prosdt;	
	ConfigLocator* ConfigL = &ConfigLocator::getInstance();
	OmSetupL oms = OmSetupL::getInstance();

	static nmainUI::statuslogs* sttlogs;
	static std::shared_mutex collectionMutex;
	static std::mutex ArtScanMutex;
	//Realtime Process variable glDatabuffer Processes
	std::vector<Color> everyColors;
	
};

class upFrame : public QOpenGLFunctions_4_3_Core, public nObserver {
public:
    upFrame() { }
    ~upFrame() { }

    void processOnGPU();

	QWidget* createFrame() override;
protected:
    void updateOffLine() override {}
    void updateRealTime() override {}

private:
    static std::unique_ptr<QOffscreenSurface> surface;
    static std::unique_ptr<QOpenGLContext> context;
    static std::unique_ptr<QOpenGLShaderProgram> computeShader;
};




#endif // NOBSERVER_H