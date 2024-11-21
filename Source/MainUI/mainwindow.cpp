#include "../pch.h"
#include "mainwindow.h"
#include "FacObsFrames/ObserverMgr.h"
#include "FacObsFrames/FactoryMgr.h"
#include "UIManager.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"
#include "MainUI/OmSettingFrame.h"

nmainUI::UIManager uiManager;
nmainUI::UIFrame::UIFrame() {}



std::shared_ptr<nSubject> nsubject;

void initSpdLog() {
    auto now = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &now);

    std::ostringstream dir_oss;
    dir_oss << "logs/logs_" << std::put_time(&tm, "%d_%m_%y");
    std::string dir_path = dir_oss.str();
    std::filesystem::create_directories(dir_path);

    std::ostringstream file_oss;
    file_oss << dir_path << "/Rop_recordlog__" << std::put_time(&tm, "[%y.%m.%d]_[%H.%M.%S]__") << ".txt";
    std::string filename = file_oss.str();

    ConfigLocator::getInstance().sysParams->tempBufferLogs = filename;

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 1024 * 1024 * 20, 10);
    auto logger = std::make_shared<spdlog::logger>("file_logger", spdlog::sinks_init_list{ file_sink });
    spdlog::set_default_logger(logger);
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%H:%M:%S] [%l] %v");
    logger->info("RopView Start");
    spdlog::flush_every(std::chrono::seconds(1));
}

int nmainUI::UIFrame::mainloop(int argc, char* argv[]) {
    initSpdLog();
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
    mainWindow->setMenuBar(static_cast<QMenuBar*>(menuBarWidget->layout()->menuBar()));

    // Free Docker
    uiManager.createLogWidget();
    OmSettingFrame::getInstance()->show();

     // Top Logs DockWidget
    QDockWidget* connectDockWidget = new QDockWidget(mainWindow);
        connectDockWidget->setObjectName("MAINUI_connectDockWidget");
        connectDockWidget->setWidget(uiManager.createConnectFrames());
        connectDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        mainWindow->addDockWidget(Qt::TopDockWidgetArea, connectDockWidget);

    // Top Logs DockWidget
    QDockWidget* logsDockWidget = new QDockWidget(mainWindow);
        logsDockWidget->setObjectName("MAINUI_logsDockWidget");
        logsDockWidget->setWidget(uiManager.createLogFrame());
        logsDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        mainWindow->addDockWidget(Qt::TopDockWidgetArea, logsDockWidget);

    // Main splitter (Vertical)
    auto mainSplitter = new QSplitter(Qt::Vertical);
        mainSplitter->setObjectName("MAINUI_mainSplitter");
        mainSplitter->setHandleWidth(0);

    // Main viewport splitter (Horizontal)
    auto mainViewportSplitter = new QSplitter(Qt::Horizontal);
        mainViewportSplitter->setObjectName("MAINUI_mainViewportSplitter");
        mainViewportSplitter->setHandleWidth(0);

    // Scan frame splitter ()
    auto scanFrameSplitter = new QSplitter(Qt::Horizontal);
        scanFrameSplitter->setObjectName("MAINUI_scanFrameSplitter");
        scanFrameSplitter->setHandleWidth(0);


    // A-scan and S-scan splitter ()
    auto BviewCview = new QSplitter(Qt::Vertical);
        BviewCview->setObjectName("MAINUI_BviewCviewSplitter");
        BviewCview->setHandleWidth(0);
        BviewCview->addWidget(uiManager.createCscanFrame());
        BviewCview->addWidget(uiManager.createBscanFrame());

    // A-scan and S-scan splitter (Horizontal)
    auto AviewSview = new QSplitter(Qt::Vertical);
        AviewSview->setObjectName("MAINUI_AviewSviewSplitter");
        AviewSview->setHandleWidth(0);
        AviewSview->addWidget(uiManager.createSscanFrame());
        AviewSview->addWidget(uiManager.createAscanFrame());

    scanFrameSplitter->addWidget(BviewCview);
    scanFrameSplitter->addWidget(AviewSview);

    mainViewportSplitter->addWidget(scanFrameSplitter);

    // 3D viewport splitter (Vertical)
    auto viewport3DSplitter = new QSplitter(Qt::Vertical);
        viewport3DSplitter->setHandleWidth(0);
        viewport3DSplitter->addWidget(uiManager.create3DFrame());

    mainViewportSplitter->addWidget(viewport3DSplitter);
    mainSplitter->addWidget(mainViewportSplitter);

    mainLayout->addWidget(mainSplitter);



    QObject::connect(&*app, &QApplication::aboutToQuit, [&]() {
        uiManager.saveWidgetState(mainWindow);
        });

    uiManager.restoreWidgetState(mainWindow);

    mainWindow->showMaximized();
    return app->exec();
}

