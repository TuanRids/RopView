
#include "IUIWindow.h"
#include <QSettings>
#include <QSplitter>
#include "SettingPara.h"
#include "Model3D/MainViewport.h"

namespace nmainUI {
    class UIManager : public UIWindow {
    public:
        UIManager();
        ~UIManager()  = default;
        QWidget* createMenuBarFrame(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject);

        QWidget* createLogFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject);
        QWidget* createLogSettings(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject);
        QWidget* createSetting2(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject);
        QWidget* createAscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject);
        QWidget* createSscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject);
        QWidget* createCscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject);
        QWidget* createBscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject);
        QWidget* create3DFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject);

        QWidget* addFrameName(const QString& name, QWidget* frame);
        QPixmap loadLogoFromResource();

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
        VulkanWindow* vulkanWindow;
        void refreshWidgets(const QList<QWidget*>& widgets);
    };
}

