#ifndef UIManager_H
#define UIManager_H
#include "../pch.h"
#include "FacObsFrames/ObserverMgr.h"
#include "FacObsFrames/nSubject.h"
#include "FacObsFrames/FactoryMgr.h"
#include "PAUTFileReader/AscanProcessor.h"
#include "Model3D/MainViewport.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/BviewFrame.h"
#include "Frames/AviewFrame.h"
#include "SystemConfig/ConfigLocator.h"
#include "../Data3DProcessing/Data3DProcess.h"


namespace nmainUI {
    class UIManager {
    public:
        UIManager();
        ~UIManager()  = default;
        QWidget* createMenuBarFrame();
        QWidget* createLogFrame();          /* LogFrame for showing all logs*/
        QWidget* createConnectFrames();
        void createLogWidget();
        QWidget* createAscanFrame();
        QWidget* createSscanFrame();
        QWidget* createCscanFrame();
        QWidget* createBscanFrame();
        QWidget* create3DFrame();

        QWidget* addFrameName(const QString& name, QWidget* frame);
        QPixmap loadLogoFromResource();

        void getUIPointers(std::shared_ptr<nSubject> gnsubject){ nsubject = gnsubject;}
        void UISETTING();
        void saveWidgetState(QWidget* widget);
        void restoreWidgetState(QWidget* widget);

    protected:
        void onNewFile() {};
        void onOpenFile();
        void onSaveFile() {};
        void onLoadSetup() ;
        void onSaveSetup() ;
        void onExit() {};
        void onCut() {};
		void onCopy() {};
		void onPaste() {};
        void onFullScreen() {};
        void openSettingsDialog();

    private:
        void showRealTimeLogs();
        VulkanWindow* vulkanWindow;
        std::shared_ptr<nSubject> nsubject;
        nmainUI::statuslogs* sttlogs = nullptr;
        QSettings* settings = nullptr;

        // multiple functions variables
        QPushButton* StartOmni;

    };
}


#endif