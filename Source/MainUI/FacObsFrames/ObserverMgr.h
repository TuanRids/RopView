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
	std::shared_mutex &getCollectionMutex() { return collectionMutex; }
	bool isGLTexture() { return nIsGlTexture.load(); }

	
	void RealDatProcess();
	void RealDatProcessGPU();
	void processOnGPU();


	size_t bufferSize() { return nAscanCollection.size(); }
	void upAscanCollector(const std::shared_ptr<IAscanCollection>& _nAscanCollection) {
		if (!_nAscanCollection) return;
		if (nAscanCollection.size() > 500) {
			nAscanCollection.clear();
			throw std::runtime_error("buffer overflow");
			return;
			// sleep to stop PAUT device automatically
		}
		nAscanCollection.push_back(_nAscanCollection);		
	}

protected:
	static QVector<VertexData> vertice_sview; // temporary
	//static QVector<VertexData> vertice_bview; // temporary
	static QVector<VertexData> vertice_cview; // temporary
	static std::atomic<bool> nIsGlTexture;

	ConfigLocator* ConfigL = &ConfigLocator::getInstance();
	OmSetupL oms = OmSetupL::getInstance();
	static bool isPanning;
	static curpt3d curpt;
	static AscanData scandat;
	static nmainUI::statuslogs* sttlogs;
	static deque<shared_ptr<IAscanCollection>> nAscanCollection;
	std::vector<Color> CreateColorPalette(int gainFactor );
	static UIArtScan* ArtScan;
	std::shared_mutex collectionMutex;
	static std::mutex ArtScanMutex;

private:
	static size_t _buffSize;
	std::vector<Color> everyColors = CreateColorPalette(ConfigL->visualConfig->Color_Palette);
	static std::vector<std::vector<cv::Point>> xySectorial;
};


class GPUProcessor : public QOpenGLFunctions_4_3_Core {
public:
	GLuint rawDataBuffer;     
	GLuint processedDataBuffer;
	QOpenGLShaderProgram computeShaderProgram;

	void initGPU() {
		initializeOpenGLFunctions();

		glGenBuffers(1, &rawDataBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawDataBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_COPY); 
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &processedDataBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, processedDataBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_COPY); 
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		computeShaderProgram.addShaderFromSourceCode(QOpenGLShader::Compute,
			R"(
            #version 430 core

            layout(local_size_x = 16, local_size_y = 16) in;

            layout(std430, binding = 0) buffer InputData {
                int rawData[];
            };

            layout(std430, binding = 1) buffer OutputData {
                float processedData[];
            };

            uniform float minAmplitude;
            uniform float maxAmplitudeSampling;
            uniform float maxAmplitudeUsable;

            void main() {
                uint idx = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x;

                // Normalize and process raw data
                float amplitude = abs(rawData[idx] - minAmplitude) / maxAmplitudeSampling * maxAmplitudeUsable;
                processedData[idx] = amplitude;
            }
            )");
		computeShaderProgram.link();
	}
};



class upFrame : public nObserver {
	QWidget* createFrame() override {return nullptr;}
	void updateOffLine() override {}
	void updateRealTime() override {}
	std::mutex upMutex;
public:

};

#endif // NOBSERVER_H