#include "mainwindow.h"
#include "ObserverMgr.h"
#include "FactoryMgr.h"
#include "UIManager.h"
#include "Frames/CscanFrame.h"
#include "Frames/BscanFrame.h"
#include "Frames/AscanFrame.h"
nmainUI::UIFrame::UIFrame() {  }
nmainUI::UIManager uiManager;


std::shared_ptr<nSubject> nsubject;

int nmainUI::UIFrame::mainloop(int argc, char* argv[]) {
    unique_ptr<QApplication> app = make_unique<QApplication>(argc, argv);
    nsubject = std::make_shared<nSubject>();

    app->setWindowIcon(QIcon(uiManager.loadLogoFromResource()));
    uiManager.UISETTING(&*app);

    // create QMainWindow
    auto mainWindow = new QMainWindow();
    auto centralWidget = new QWidget(mainWindow);
    mainWindow->setCentralWidget(centralWidget);

    // create layout chính
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // create menu bar 
    QWidget* menuBarWidget = uiManager.createMenuBarFrame(this, nsubject);
    mainLayout->setMenuBar(static_cast<QMenuBar*>(menuBarWidget->layout()->menuBar()));

    // Main splitter (Vertical)
    auto mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->setObjectName("mainSplitter");
    mainSplitter->setHandleWidth(0);

    // Top Logs splitter (Horizontal)
    auto TopLogsSplitter = new QSplitter(Qt::Horizontal);
    TopLogsSplitter->setObjectName("TopLogsSplitter");
    TopLogsSplitter->setHandleWidth(20);
    TopLogsSplitter->addWidget(uiManager.createLogFrame(this, nsubject));
    TopLogsSplitter->addWidget(uiManager.createLogSettings(this, nsubject));
    TopLogsSplitter->addWidget(uiManager.createSetting2(this, nsubject));
    mainSplitter->addWidget(TopLogsSplitter);

    // Main viewport splitter (Horizontal)
    auto mainViewportSplitter = new QSplitter(Qt::Horizontal);
    mainViewportSplitter->setObjectName("mainViewportSplitter");
    mainViewportSplitter->setHandleWidth(0);

    // Scan frame splitter (Vertical)
    auto scanFrameSplitter = new QSplitter(Qt::Vertical);

    // A-scan and B-scan splitter (Horizontal)
    auto ascanBscanSplitter = new QSplitter(Qt::Horizontal);
    ascanBscanSplitter->setObjectName("ascanBscanSplitter");
    ascanBscanSplitter->setHandleWidth(0);
    ascanBscanSplitter->addWidget(uiManager.createAscanFrame(this, nsubject));
    ascanBscanSplitter->addWidget(uiManager.createBscanFrame(this, nsubject));

    scanFrameSplitter->addWidget(ascanBscanSplitter);
    scanFrameSplitter->addWidget(uiManager.createCscanFrame(this, nsubject));

    mainViewportSplitter->addWidget(scanFrameSplitter);

    // 3D viewport splitter (Vertical)
    auto viewport3DSplitter = new QSplitter(Qt::Vertical);
    viewport3DSplitter->setObjectName("viewport3DSplitter");
    viewport3DSplitter->setHandleWidth(0);
    viewport3DSplitter->addWidget(uiManager.create3DFrame(this, nsubject));

    mainViewportSplitter->addWidget(viewport3DSplitter);
    mainSplitter->addWidget(mainViewportSplitter);

    // Thêm splitter vào layout chính
    mainLayout->addWidget(mainSplitter);

    mainWindow->showMaximized();

    QObject::connect(&*app, &QApplication::aboutToQuit, [&]() {
        uiManager.saveWidgetState(mainWindow);
        });

    uiManager.restoreWidgetState(mainWindow);

    return app->exec();
}


void nmainUI::UIFrame::logical() {

    if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    if (!processor) { processor = std::make_shared<AscanProcessor>(); }
    if (processor) 
    { 
        auto factframe = nFactoryFrame::crCscanFrm(this);
        factframe->clearScandat();
        auto res = processor->analyze(&*factframe);
        factframe->setSttlogs();
        if (res) { sttlogs->logInfo("Scanning Data is Loaded."); }
        factframe->setter_Curpt(1, 1, 1);
        nsubject->notify();
    }    
}

void nmainUI::UIFrame::refreshxyz() 
{
    nsubject->notify(); 
}
