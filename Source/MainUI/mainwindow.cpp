#include "mainwindow.h"
#include "ObserverMgr.h"
#include "FactoryMgr.h"
#include "..\Source\Frames\nLogFrame.h"
#include "..\Source\Frames\CscanFrame.h"
#include "Frames/BscanFrame.h"
#include "Frames/BscanFrame.h"
nmainUI::UIFrame::UIFrame() : app(nullptr), mainWindow(nullptr), centralWidget(nullptr), splitter(nullptr),
                              button(nullptr), sttlogs(nullptr), graphicsView(nullptr), scene(nullptr), processor(nullptr) 
{ processor = std::make_shared<AscanProcessor>(); }

void nmainUI::UIFrame::UISETTING()
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

std::shared_ptr<CscanFrame> cScanFrm;
std::shared_ptr<BscanFrame> bScanFrm;
std::shared_ptr<nLogFrame> logframe;
std::shared_ptr<nSubject> nsubject;

int nmainUI::UIFrame::mainloop(int argc, char* argv[]) {
    QApplication appct(argc, argv);
    this->app = &appct;
    nsubject = std::make_shared<nSubject>();

    app->setWindowIcon(QIcon(loadLogoFromResource()));
    UISETTING();
    // Create main window
    mainWindow = new QMainWindow();
    mainWindow->setWindowTitle("RoqView");

    // Create central widget
    centralWidget = new QWidget(mainWindow);
    mainWindow->setCentralWidget(centralWidget);

    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    centralWidget->setLayout(layout);
    mainWindow->resize(1080, 720);
    mainWindow->showMaximized();

    // Create splitter
    splitter = new QSplitter(Qt::Vertical);

    // Create Layout 1 (contains logframe and button in horizontal layout)
    QWidget* layout1Widget = new QWidget();
    QHBoxLayout* layout1 = new QHBoxLayout(layout1Widget);

    logframe = nFactoryFrame::createLogFrame();
    nsubject->addObserver(logframe);
    layout1->addWidget(logframe->createFrame());

    QPushButton* btn = new QPushButton("Load");
    btn->setFixedSize(100, 40);
    QObject::connect(btn, &QPushButton::clicked, [=]() mutable {
        logical(&*cScanFrm); });
    layout1->addWidget(btn);

    // Create Layout 2 (contains cScanFrm and bScanFrm in horizontal layout)
    QSplitter* splitter2= new QSplitter(Qt::Horizontal);
    QWidget* layout2Widget = new QWidget();
    QHBoxLayout* layout2 = new QHBoxLayout(layout2Widget);

    cScanFrm = nFactoryFrame::createGraphicsFrame();
    cScanFrm->setUIFrame(this);
    nsubject->addObserver(cScanFrm);
    splitter2->addWidget(cScanFrm->createFrame());

    bScanFrm = nFactoryFrame::createBscanFrame();
    bScanFrm->setUIFrame(this);
    nsubject->addObserver(bScanFrm);
    splitter2->addWidget(bScanFrm->createFrame());

    // Add layouts to the splitter
    splitter->addWidget(layout1Widget);
    splitter->addWidget(splitter2);

    nsubject->notify(-1, -1, -1);
    mainWindow->show();

    // Save and restore splitter sizes
    QSettings settings("RoqView COM", "RoqView APP");
    QObject::connect(app, &QApplication::aboutToQuit, [&]() {
        settings.setValue("splitterSizes", splitter->saveState()); });
    QByteArray splitterState = settings.value("splitterSizes").toByteArray();
    splitter->restoreState(splitterState);

    layout->addWidget(splitter);
    return app->exec();
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


void nmainUI::UIFrame::logical(nFrame* frame) {

    if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    if (processor) 
    { 
        auto res = processor->analyze(frame); 
        frame->setSttlogs();
        if (res) { sttlogs->logInfo("Scanning Data is Loaded."); }
        nsubject->notify(1,1,1);
    }

}

void nmainUI::UIFrame::refreshxyz(const int x, const int y, const int z)
{
    nsubject->notify(x,y,z); 
}
