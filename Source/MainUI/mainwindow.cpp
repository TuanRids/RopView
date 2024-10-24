#include "../pch.h"
#include "mainwindow.h"
#include "ObserverMgr.h"
#include "FactoryMgr.h"
#include "UIManager.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"


nmainUI::UIManager uiManager;
nmainUI::UIFrame::UIFrame() 
{ }



std::shared_ptr<nSubject> nsubject;

int nmainUI::UIFrame::mainloop(int argc, char* argv[]) {
    app = new QApplication(argc, argv);
    nsubject = std::make_shared<nSubject>();
    uiManager.getUIPointers(nsubject);
    app->setWindowIcon(QIcon(uiManager.loadLogoFromResource()));
    uiManager.UISETTING();

    // create QMainWindow
    auto mainWindow = new QMainWindow();
    auto centralWidget = new QWidget(mainWindow);
    mainWindow->setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // create menu bar 
    QWidget* menuBarWidget = uiManager.createMenuBarFrame();
    mainLayout->setMenuBar(static_cast<QMenuBar*>(menuBarWidget->layout()->menuBar()));

    // Main splitter (Vertical)
    auto mainSplitter = new QSplitter(Qt::Vertical);
        mainSplitter->setObjectName("mainSplitter");
        mainSplitter->setHandleWidth(0);

    // Top Logs splitter (Horizontal)
    auto TopLogsSplitter = new QSplitter(Qt::Horizontal);
        TopLogsSplitter->setObjectName("TopLogsSplitter");
        TopLogsSplitter->setHandleWidth(20);
        TopLogsSplitter->addWidget(uiManager.createLogFrame());
        TopLogsSplitter->addWidget(uiManager.createLogSettings());
        TopLogsSplitter->addWidget(uiManager.createSetting2());
        TopLogsSplitter->addWidget(uiManager.createLogDebug());
        mainSplitter->addWidget(TopLogsSplitter);

    // Main viewport splitter (Horizontal)
    auto mainViewportSplitter = new QSplitter(Qt::Horizontal);
        mainViewportSplitter->setObjectName("mainViewportSplitter");
        mainViewportSplitter->setHandleWidth(0);

    // Scan frame splitter ()
    auto scanFrameSplitter = new QSplitter(Qt::Horizontal);

    // A-scan and S-scan splitter ()
    auto BviewCview = new QSplitter(Qt::Vertical);
        BviewCview->setObjectName("BviewCview");
        BviewCview->setHandleWidth(0);
        BviewCview->addWidget(uiManager.createCscanFrame());
        BviewCview->addWidget(uiManager.createBscanFrame());

    // A-scan and S-scan splitter (Horizontal)
    auto AviewSview = new QSplitter(Qt::Vertical);
        AviewSview->setObjectName("AscanSscanSplitter");
        AviewSview->setHandleWidth(0);
        AviewSview->addWidget(uiManager.createSscanFrame());
        AviewSview->addWidget(uiManager.createAscanFrame());

    scanFrameSplitter->addWidget(BviewCview);
    scanFrameSplitter->addWidget(AviewSview);

    mainViewportSplitter->addWidget(scanFrameSplitter);

    // 3D viewport splitter (Vertical)
    auto viewport3DSplitter = new QSplitter(Qt::Vertical);
        viewport3DSplitter->setObjectName("viewport3DSplitter");
        viewport3DSplitter->setHandleWidth(0);
        viewport3DSplitter->addWidget(uiManager.create3DFrame());

    mainViewportSplitter->addWidget(viewport3DSplitter);
    mainSplitter->addWidget(mainViewportSplitter);

    mainLayout->addWidget(mainSplitter);

    mainWindow->showMaximized();

    QObject::connect(&*app, &QApplication::aboutToQuit, [&]() {
        uiManager.saveWidgetState(mainWindow);
        });

    uiManager.restoreWidgetState(mainWindow);

    return app->exec();
}

