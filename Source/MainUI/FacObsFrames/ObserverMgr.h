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
class upFrame;
class PAUTManager;

class nObserver {
public:
	nObserver();
	virtual ~nObserver() = default;

	virtual QWidget* createFrame() = 0;
	virtual void updateOffLine() = 0;
	virtual void updateRealTime() = 0;

	void setScandat(const AscanData& dataa) { scandat = dataa; }
	void clsScanDat() { scandat = AscanData(); }

protected:
	static PAUTManager* pautmgr;
	static std::vector<Color> everyColors;
	static AscanData scandat;
	static nmainUI::statuslogs* sttlogs;
	static bool isPanning;
	static int curpt[3];
	ConfigLocator* ConfigL = &ConfigLocator::getInstance();
	OmSetupL oms = OmSetupL::getInstance();
};



class PAUTManager : public QOpenGLFunctions_4_3_Core {
public:
	// Singleton Access
	static PAUTManager& getInstance() {
		static PAUTManager instance;
		return instance;
	}
	~PAUTManager() = default;

	PAUTManager(const PAUTManager&) = delete;
	PAUTManager& operator=(const PAUTManager&) = delete;

	//process
	void RealDatProcess();
	void processOnGPU(); // use childClass to process by OpenGL4_3 for avoiding multi inhertitance

	void clearAll() {
		std::unique_lock<std::shared_mutex> lock(pautMutex);
		prosdt.ArtScan->resetAll(); prosdt.nAscanCollection.clear();
	}

	void upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection);
	void waitAscan() { prosdt.nAscanCollection.checkOrWait(); }

	// getter & check
	bool isGLTexture() { return prosdt.nIsGlTexture.load(); }
	std::shared_mutex& getCollectionMutex() { return pautMutex; }
	size_t bufferSize() { return prosdt.nAscanCollection.size(); }

	// public for avoiding too much setter. can be private if have time
	ProcessingContext prosdt; 
	std::shared_mutex pautMutex;
	std::mutex ArtScanMutex;
private:
	PAUTManager();

	void initializeGLResources();

	nmainUI::statuslogs* sttlogs;
	//Realtime Process variable glDatabuffer Processes
	std::vector<Color> everyColors;

	std::unique_ptr<QOffscreenSurface> surface;
	std::unique_ptr<QOpenGLContext> context;
	std::unique_ptr<QOpenGLShaderProgram> computeShader;
	ConfigLocator* ConfigL = &ConfigLocator::getInstance();
	OmSetupL oms = OmSetupL::getInstance();

	void updateParameters(std::shared_ptr<IAscanCollection>& RawAsanDat);
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