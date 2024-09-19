#include "mainwindow.h"
#include "ObserverMgr.h"
#include "FactoryMgr.h"
#include "Frames/nLogFrame.h"
#include "Frames/CscanFrame.h"
#include "Frames/BscanFrame.h"
#include "Frames/AscanFrame.h"
nmainUI::UIFrame::UIFrame() : sttlogs(nullptr),  processor(nullptr) 
{ processor = std::make_shared<AscanProcessor>(); }

void nmainUI::UIFrame::UISETTING(QApplication* app)
{
    app->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app->setPalette(darkPalette);

    app->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
        "QPushButton { background-color: #353535; color: white; border: 1px solid #2a82da; padding: 5px; }"
        "QPushButton:hover { background-color: #2a82da; }"
        "QLineEdit { background-color: #353535; color: white; border: 1px solid #2a82da; }"
        "QComboBox { background-color: #353535; color: white; border: 1px solid #2a82da; }"
        "QComboBox QAbstractItemView { background-color: #353535; }");
}

std::shared_ptr<nSubject> nsubject;


int nmainUI::UIFrame::mainloop(int argc, char* argv[]) {
    unique_ptr<QApplication> app = make_unique<QApplication>(argc, argv);
    nsubject = std::make_shared<nSubject>();

    app->setWindowIcon(QIcon(loadLogoFromResource()));
    UISETTING(&*app);

    // Create main window
    auto mainWindow = new QMainWindow();
    mainWindow->setWindowTitle("RoqView");

    // Create central widget
    auto centralWidget = new QWidget(mainWindow);
    mainWindow->setCentralWidget(centralWidget);

    // Set layout for the central widget
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    centralWidget->setLayout(layout);
    mainWindow->resize(1080, 720);
    mainWindow->showMaximized();

    // Create the overall splitter
    auto splitter = new QSplitter(Qt::Vertical);

    // Add frames to splitter using helper functions
    splitter->addWidget(createLogFrame());
    splitter->addWidget(createCScanBScanFrame());

    splitter->addWidget(createAscanFrame());


    nsubject->notify();
    mainWindow->show();

    // Save and restore splitter sizes
    QSettings settings("RoqView COM", "RoqView APP");
    QObject::connect(&*app, &QApplication::aboutToQuit, [&]() {
        settings.setValue("splitterSizes", splitter->saveState());
        });
    QByteArray splitterState = settings.value("splitterSizes").toByteArray();
    splitter->restoreState(splitterState);

    layout->addWidget(splitter);
    return app->exec();
}

QWidget* nmainUI::UIFrame::createLogFrame() {
    // Create Log Frame layout
    QWidget* logWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(logWidget);

    auto logframe = nFactoryFrame::crLogFrm();
    nsubject->addObserver(logframe);
    layout->addWidget(logframe->createFrame());

    // Add a button (example for log)
    QPushButton* btn = new QPushButton("Load");
    btn->setFixedSize(100, 40);
    QObject::connect(btn, &QPushButton::clicked, [=]() mutable {
        logical();
        });
    layout->addWidget(btn);

    return logWidget;
}

QWidget* nmainUI::UIFrame::createAscanFrame() {
    // Create Ascan Frame layout
    QWidget* ascanWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(ascanWidget);

    auto aScanFrm = nFactoryFrame::crAscanFrm(this);
    nsubject->addObserver(aScanFrm);
    layout->addWidget(aScanFrm->createFrame());

    return ascanWidget;
}

QWidget* nmainUI::UIFrame::createCScanBScanFrame() {
    // Create layout for Cscan and Bscan
    QSplitter* splitter2 = new QSplitter(Qt::Horizontal);
    QWidget* frameWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(frameWidget);

    auto cScanFrm = nFactoryFrame::crCscanFrm(this);
    nsubject->addObserver(cScanFrm);
    splitter2->addWidget(cScanFrm->createFrame());

    auto bScanFrm = nFactoryFrame::crBscanFrm(this);
    nsubject->addObserver(bScanFrm);
    splitter2->addWidget(bScanFrm->createFrame());

    layout->addWidget(splitter2);

    return frameWidget;
}

QPixmap nmainUI::UIFrame::loadLogoFromResource()
{
    
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(107), RT_RCDATA);
    if (hRes) {
        HGLOBAL hData = LoadResource(NULL, hRes);
        if (hData) {
            DWORD dataSize = SizeofResource(NULL, hRes);
            const char* data = static_cast<const char*>(LockResource(hData));
            if (data) {
                QByteArray byteArray(data, dataSize);
                QPixmap pixmap;
                pixmap.loadFromData(byteArray, "PNG");  
                return pixmap;
            }
        }
    }
    return QPixmap();
}

void nmainUI::UIFrame::logical() {

    if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    if (processor) 
    { 
        auto factframe = nFactoryFrame::crCscanFrm(this);
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
