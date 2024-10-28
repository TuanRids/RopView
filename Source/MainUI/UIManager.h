#ifndef UIManager_H
#define UIManager_H
#include <QSettings>
#include <QSplitter>
#include "FacObsFrames/ObserverMgr.h"
#include "FacObsFrames/nSubject.h"
#include "PAUTFileReader/AscanProcessor.h"

#include "Model3D/MainViewport.h"

namespace nmainUI {
    class UIManager {
    public:
        UIManager();
        ~UIManager()  = default;
        QWidget* createMenuBarFrame();

        QWidget* createLogFrame();
        QWidget* createLogSettings();
        QWidget* createSetting2();
        QWidget* createLogDebug();
        QWidget* createOmSetting();
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
        void onOpenFile() {};
        void onSaveFile() {};
        void onExit() {};
        void onCut() {};
		void onCopy() {};
		void onPaste() {};
        void onFullScreen() {};
        void openSettingsDialog();
    private:
        void showRealTimeLogs();
        void logical();
        VulkanWindow* vulkanWindow;
        void refreshWidgets(const QList<QWidget*>& widgets);
        std::shared_ptr<nSubject> nsubject;
        nmainUI::statuslogs* sttlogs = nullptr;
        std::unique_ptr<QSettings> settings = nullptr;
    };
}


#endif