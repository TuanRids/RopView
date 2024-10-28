#include "UIManager.h"
#include <QApplication>
#include <QSplitter>
#include <QVBoxLayout>
#include <QTimer>

#include "FacObsFrames/FactoryMgr.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/BviewFrame.h"
#include "Frames/AviewFrame.h"
#include "SystemConfig/ConfigLocator.h"
#include "../Data3DProcessing/Data3DProcess.h"
#include "OmConnect/OmCreateSetupSetting/OmCreateSetup.h"
QMainWindow* getMainWindow() {
    foreach(QWidget * widget, QApplication::topLevelWidgets()) {
        if (QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget)) {
            return mainWindow;
        }
    }
    return nullptr;
}
namespace nmainUI {
    UIManager::UIManager(): sttlogs(nullptr), nsubject(nullptr){
        settings = std::make_unique<QSettings>("RoqView COM", "FrameStatus");
        if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    }
    QPixmap UIManager::loadLogoFromResource() {
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
    void UIManager::saveWidgetState(QWidget* widget) {
        QList<QSplitter*> splitters = widget->findChildren<QSplitter*>();
        for (auto splitter : splitters) {
            settings->setValue(splitter->objectName() + "_splitterState", splitter->saveState());
        }
    }
    void UIManager::restoreWidgetState(QWidget* widget) {
        QList<QSplitter*> splitters = widget->findChildren<QSplitter*>();
        for (auto splitter : splitters) {
            QByteArray splitterState = settings->value(splitter->objectName() + "_splitterState").toByteArray();
            if (!splitterState.isEmpty()) {
                splitter->restoreState(splitterState);
            }
        }
        widget->update(); 
    }


    QWidget* UIManager::createMenuBarFrame()
    {
        // Create a main widget for the menu bar
        QWidget* menuBarWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(menuBarWidget);
        layout->setContentsMargins(0, 0, 0, 0);

        // Create a QMenuBar
        QMenuBar* menuBar = new QMenuBar(menuBarWidget);

        // Create some menus
        QMenu* fileMenu = menuBar->addMenu("File");
        QMenu* editMenu = menuBar->addMenu("Edit");
        QMenu* viewMenu = menuBar->addMenu("View");

        // Add actions (you can link them to actual functionalities later)
        fileMenu->addAction("New", [=]() { onNewFile(); });
        fileMenu->addAction("Open", [=]() { onOpenFile(); });
        fileMenu->addAction("Save", [=]() { onSaveFile(); });
        fileMenu->addAction("Setting", [=]() { openSettingsDialog(); });
        fileMenu->addAction("Exit", [=]() { onExit(); });

        editMenu->addAction("Cut", [=]() { onCut(); });
        editMenu->addAction("Copy", [=]() { onCopy(); });
        editMenu->addAction("Paste", [=]() { onPaste(); });

        viewMenu->addAction("FullScreen", [=]() { onFullScreen(); });
        viewMenu->addAction("Debug Logs", [=]() { showRealTimeLogs(); });


        // Add the QMenuBar to the layout
        layout->setMenuBar(menuBar);

        return menuBarWidget;
    }

    void UIManager::showRealTimeLogs() {
        auto nmainwd = getMainWindow();
        QDockWidget* logDockWidget = new QDockWidget("Real-Time Logs", nmainwd);
        QTextEdit* logTextEdit = new QTextEdit(logDockWidget);
        logTextEdit->setReadOnly(true);
        logDockWidget->setWidget(logTextEdit);
        nmainwd->addDockWidget(Qt::RightDockWidgetArea, logDockWidget);

        QString logFilePath = QString::fromStdString(ConfigLocator::getInstance().sysParams->tempBufferLogs);
        static qint64 lastPos = 0;
        const int maxLogSize = 10000;  

        QTimer* logUpdateTimer = new QTimer(nmainwd);
        QObject::connect(logUpdateTimer, &QTimer::timeout, [logTextEdit, logFilePath]() mutable {
            QFile file(logFilePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                file.seek(lastPos);
                QTextStream in(&file);
                QString newContent = in.readAll();
                lastPos = file.pos();
                file.close();

                if (!newContent.isEmpty()) {
                    if (logTextEdit->toPlainText().size() > maxLogSize) {
                        logTextEdit->clear();
                    }

                    logTextEdit->moveCursor(QTextCursor::End);
                    logTextEdit->insertPlainText(newContent);
                    logTextEdit->moveCursor(QTextCursor::End);
                }
            }
            });

        logUpdateTimer->start(1000); 
        logDockWidget->show();

    }

    void UIManager::refreshWidgets(const QList<QWidget*>& widgets) {
        for (QWidget* widget : widgets) {
            widget->setStyleSheet(qApp->styleSheet());
            widget->update();
        }
    }
    void UIManager::UISETTING() {
        auto gtheme = ConfigLocator::getInstance().settingconf->qsTheme;
        auto sheetName = fs::current_path() / ("QssTemplate\\"+ gtheme);
        QString styleSheet;

        // Load QSS file if exists
        QFile file(sheetName);
        if (file.open(QFile::ReadOnly)) {
            styleSheet = QLatin1String(file.readAll());
        }

        //// Dark palette setup (this will be overridden by QSS if conflicting)
        //QPalette darkPalette;
        //darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        //darkPalette.setColor(QPalette::WindowText, Qt::white);
        //darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
        //darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        //darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        //darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        //darkPalette.setColor(QPalette::Text, Qt::white);
        //darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        //darkPalette.setColor(QPalette::ButtonText, Qt::white);
        //darkPalette.setColor(QPalette::BrightText, Qt::red);
        //darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        //darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        //darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        //app->setPalette(darkPalette);

        //// Additional styles (append these to the existing stylesheet)
        //styleSheet += "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
        //    "QPushButton { background-color: #353535; color: white; border: 1px solid #2a82da; padding: 5px; }"
        //    "QPushButton:hover { background-color: #2a82da; }"
        //    "QLineEdit { background-color: #353535; color: white; border: 1px solid #2a82da; }"
        //    "QComboBox { background-color: #353535; color: white; border: 1px solid #2a82da; }"
        //    "QComboBox QAbstractItemView { background-color: #353535; }";

        // Apply the combined stylesheet once
        qApp->setStyleSheet(styleSheet);
        refreshWidgets(qApp->allWidgets());

    }

    void UIManager::openSettingsDialog()
    {
        auto currentSettings = ConfigLocator::getInstance().settingconf;

        QDialog settingsDialog;
        settingsDialog.setWindowTitle("Settings");

        QHBoxLayout* mainLayout = new QHBoxLayout(&settingsDialog);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* applyButton = new QPushButton("Apply", &settingsDialog);
        applyButton->setFixedSize(100, 30);
        buttonLayout->addWidget(applyButton);

        // Group Box to hold all settings with fixed size
        {
            QGroupBox* settingsGroup = new QGroupBox("Omniscan Settings", &settingsDialog);
            settingsGroup->setFixedSize(200, 200);
            QFormLayout* formLayout = new QFormLayout(settingsGroup);

            // Create widgets to adjust settings
            QCheckBox* triggerCheckBox = new QCheckBox("Bscan H_Res", settingsGroup);
            triggerCheckBox->setToolTip("High Resolution Bscan");
            triggerCheckBox->setChecked(currentSettings->bhighResBscan);
            formLayout->addRow(triggerCheckBox);

            QLineEdit* ipAddressField = new QLineEdit(QString::fromStdString(currentSettings->ipAddress), settingsGroup);
            ipAddressField->setToolTip("Enter the IP Address");
            formLayout->addRow(new QLabel("IP Address:"), ipAddressField);

            QSpinBox* portSpinBox = new QSpinBox(settingsGroup);
            portSpinBox->setRange(1, 65535);
            portSpinBox->setValue(currentSettings->port);
            portSpinBox->setToolTip("Set the Port number");
            formLayout->addRow(new QLabel("Port:"), portSpinBox);

            QSpinBox* timeoutSpinBox = new QSpinBox(settingsGroup);
            timeoutSpinBox->setRange(1000, 60000);
            timeoutSpinBox->setValue(currentSettings->timeout);
            timeoutSpinBox->setToolTip("Set the timeout in milliseconds");
            formLayout->addRow(new QLabel("Timeout (ms):"), timeoutSpinBox);

            QLineEdit* deviceNameField = new QLineEdit(QString::fromStdString(currentSettings->device_name), settingsGroup);
            deviceNameField->setToolTip("Enter the device name");
            formLayout->addRow(new QLabel("Device Name:"), deviceNameField);

            // Add the group box to the main layout
            mainLayout->addWidget(settingsGroup);

            QObject::connect(applyButton, &QPushButton::clicked, [&]() {
                currentSettings->bhighResBscan = triggerCheckBox->isChecked();
                currentSettings->ipAddress = ipAddressField->text().toStdString();
                currentSettings->port = portSpinBox->value();
                currentSettings->timeout = timeoutSpinBox->value();
                currentSettings->device_name = deviceNameField->text().toStdString();

                ConfigLocator::getInstance().settingconf = currentSettings;
                ConfigLocator::getInstance().saveToRegistry();
                settingsDialog.accept();
                });
        }
        // Group Box Other settings
        {
            QGroupBox* settingsGroup = new QGroupBox("Others", &settingsDialog);
            settingsGroup->setFixedSize(200, 200);
            QFormLayout* formLayout = new QFormLayout(settingsGroup);

            // Dropdown list (ComboBox) for themes
            QComboBox* themeComboBox = new QComboBox(settingsGroup);
            themeComboBox->setToolTip("Select UI Theme");

            // Load all QSS files from QssTemplate folder
            fs::path qssFolder = fs::current_path() / "QssTemplate";
            if (fs::exists(qssFolder) && fs::is_directory(qssFolder)) {
                for (const auto& entry : fs::directory_iterator(qssFolder)) {
                    if (entry.path().extension() == ".qss") {
                        themeComboBox->addItem(QString::fromStdString(entry.path().filename().string()));
                    }
                }
            }
            

            // Set current theme selection
            themeComboBox->setCurrentText(QString::fromStdString(currentSettings->qsTheme));

            formLayout->addRow(new QLabel("Theme:"), themeComboBox);
            
            QPushButton* reloadTheme = new QPushButton("Hotload");
            formLayout->addWidget(reloadTheme);
            QObject::connect(reloadTheme, &QPushButton::clicked, [&]() {
                currentSettings->qsTheme = themeComboBox->currentText().toStdString();
                ConfigLocator::getInstance().settingconf = currentSettings;
                UISETTING();
                sttlogs->logInfo("Reload to use theme: " + currentSettings->qsTheme);
                });

            mainLayout->addWidget(settingsGroup);

            QObject::connect(applyButton, &QPushButton::clicked, [&]() {
                ConfigLocator::getInstance().saveToRegistry();
                UISETTING();
                sttlogs->logInfo("Settings applied.");
                });
            
        }


        mainLayout->addLayout(buttonLayout);

        settingsDialog.exec();
    }

    void UIManager::logical()
    {
        if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
        static std::shared_ptr<AscanProcessor> processor;
        if (!processor) { processor = std::make_shared<AscanProcessor>(); }
        if (processor)
        {
            auto factframe = nFactoryFrame::crCviewFrm();
            factframe->clearScandat();
            auto res = processor->analyze(&*factframe);
            if (res) { sttlogs->logInfo("Scanning Data is Loaded."); }
            factframe->setter_Curpt(1, 1, 1);
            nsubject->startNotifyTimer(52);
        }
    }
    QWidget* UIManager::addFrameName(const QString& name, QWidget* frame) {
        if (!frame->layout()) {
            QVBoxLayout* layout = new QVBoxLayout(frame);
            layout->setContentsMargins(0, 0, 0, 0);
        }

        QLabel* label = new QLabel(name, frame);
        label->setStyleSheet("color: red; font-weight: bold; background-color: rgba(0, 0, 0, 0);"); 
        label->setGeometry(30, 10, 100, 20); 

        label->setAttribute(Qt::WA_TransparentForMouseEvents);
        label->raise();
        return frame;
    }

    QWidget* UIManager::createLogFrame() {
        QWidget* logWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(logWidget);
        layout->setContentsMargins(0, 0, 0, 0);

        QTextEdit* logOutput = new QTextEdit();
        logOutput->setLineWrapMode(QTextEdit::NoWrap);
        logOutput->setReadOnly(true);

        nmainUI::statuslogs::getinstance().initialize(logOutput);

        layout->addWidget(logOutput);

        return logWidget;
    }
    QWidget* UIManager::createLogSettings() {
        QWidget* settingsWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(settingsWidget);
        layout->setContentsMargins(1, 1, 1, 1);

        // Resolution setting
        auto resInput = new QSpinBox();
        resInput->setRange(1, 100);
        resInput->setValue(ConfigLocator::getInstance().sysParams->resolution);
        auto resLayout = new QHBoxLayout();
        resLayout->addWidget(new QLabel("Res:"));
        resLayout->addWidget(resInput);
        layout->addLayout(resLayout);

        QObject::connect(resInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) mutable {
            ConfigLocator::getInstance().sysParams->resolution = resInput->value();
            nsubject->stopRealtimeUpdate();
            });

        // Add a button
        QPushButton* btnLoad = new QPushButton("Load Offline");
        layout->addWidget(btnLoad);
        QObject::connect(btnLoad, &QPushButton::clicked, [=]() mutable {
            nsubject->stopRealtimeUpdate();
            nsubject->startNotifyTimer(52);
            IOmConnect::Create()->omDisconnectDevice();
            logical();
            });
        // Add a button
        QPushButton* btnSaveSetup = new QPushButton("Save Setup");
        layout->addWidget(btnSaveSetup);
        QObject::connect(btnSaveSetup, &QPushButton::clicked, [=]() mutable {
            if (OmCreateSetup::SaveSetup()) sttlogs->logCritical("Save Setup!");
            else sttlogs->logCritical("Save Setup Failed!");
            });
        // Add a button
        QPushButton* btnConnect = new QPushButton("Connect");
        layout->addWidget(btnConnect);
        QObject::connect(btnConnect, &QPushButton::clicked, [=]() mutable {
            nsubject->stopNotifyTimer();
            auto resHz = 1000.0f / (ConfigLocator::getInstance().omconf->Rate ) > 10 ? 1000.0f / (ConfigLocator::getInstance().omconf->Rate + 10) : 5.0f;
            if (IOmConnect::Create()->omConnectDevice()) sttlogs->logCritical("Start RealTime!");
            nsubject->startRealtimeUpdate( 45 ); //NOTE: refresh rate for realtime rendering
            });
        // add 3D button
		QPushButton* btn3D = new QPushButton("3D");
		layout->addWidget(btn3D);
        QObject::connect(btn3D, &QPushButton::clicked, [=]() mutable {
            std::unique_ptr<IData3DProcecss> object = std::make_unique<Data3DProcess>();
            auto result = object->processData();
            vulkanWindow->PautINIT(result);
            });
         
        
        // Add a checkbox
        QCheckBox* checkBox = new QCheckBox("Cscan Layer");
        checkBox->setCheckState(Qt::Unchecked);
        layout->addWidget(checkBox);
        QObject::connect(checkBox, &QCheckBox::stateChanged, [=](int state) {
            ConfigLocator::getInstance().sysParams->isCscanLayer = (state == Qt::Checked);
            });

        return settingsWidget;
    }
    QWidget* UIManager::createSetting2()
    {
        QWidget* logWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(logWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        return logWidget;
    }
    QWidget* UIManager::createLogDebug()
    {
        QWidget* logWidget = new QWidget();
        QTimer* timer = new QTimer(logWidget);
        QVBoxLayout* layout = new QVBoxLayout(logWidget);
        layout->setContentsMargins(0, 0, 0, 0);

        QTextEdit* logOutput = new QTextEdit();
        logOutput->setLineWrapMode(QTextEdit::NoWrap);
        logOutput->setReadOnly(true);
        QObject::connect(timer, &QTimer::timeout, [logOutput]() {
            static auto setthoughout = &spdThoughout::getinstance();
            size_t buffer = std::make_shared<upFrame>()->bufferSize();
            logOutput->clear();
            QString text = QString("Buffer Size: %1\n").arg(static_cast<int>(buffer));
            text += QString("Throughput Mbs: %1\n").arg(setthoughout->get());
            text += QString("AllScan TimeCycle ms: %1\n").arg(setthoughout->get_FPS());
            logOutput->append(text);  
            });

        timer->start(50); 
        layout->addWidget(logOutput);
        return logWidget;
    }

    QWidget* UIManager::createOmSetting() {
        auto omc = ConfigLocator::getInstance().omconf;
        QWidget* settingsWidget = new QWidget();
        QTabWidget* tabWidget = new QTabWidget(settingsWidget);

        auto addSetting = [](QVBoxLayout* layout, const QString& label, QWidget* widget) {
            QHBoxLayout* hLayout = new QHBoxLayout();
            hLayout->addWidget(new QLabel(label));
            hLayout->addWidget(widget);
            layout->addLayout(hLayout);
            };

        // Tab 1
        QWidget* tab1 = new QWidget();
        QVBoxLayout* tab1Layout = new QVBoxLayout(tab1);

        // Group 1 - Beam Settings
        QGroupBox* group1 = new QGroupBox("Beam Settings");
        QVBoxLayout* layout1 = new QVBoxLayout(group1);

        auto beamPositionInput = new QSpinBox();
        beamPositionInput->setRange(0, 100);
        beamPositionInput->setValue(static_cast<int>(omc->BeamPosition));
        beamPositionInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout1, "Beam Position:", beamPositionInput);
        QObject::connect(beamPositionInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->BeamPosition = value;
            });

        auto rateInput = new QSpinBox();
        rateInput->setRange(10, 1000);
        rateInput->setValue(static_cast<int>(omc->Rate));
        rateInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout1, "Rate (Hz):", rateInput);
        QObject::connect(rateInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->Rate = value;
            });

        auto beamLimitInput = new QSpinBox();
        beamLimitInput->setRange(1, 512);
        beamLimitInput->setValue(static_cast<int>(omc->beamLimit));
        beamLimitInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout1, "Beam Limit:", beamLimitInput);
        QObject::connect(beamLimitInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->beamLimit = value;
            });

        auto apertureInput = new QSpinBox();
        apertureInput->setRange(1, 64);
        apertureInput->setValue(static_cast<int>(omc->elementAperture));
        apertureInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout1, "Element Aperture:", apertureInput);
        QObject::connect(apertureInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->elementAperture = value;
            });

        tab1Layout->addWidget(group1);

        // Group 2 - Delay Settings
        QGroupBox* group2 = new QGroupBox("Delay Settings");
        QVBoxLayout* layout2 = new QVBoxLayout(group2);

        auto delayResolutionInput = new QDoubleSpinBox();
        delayResolutionInput->setRange(0.1, 20.0);
        delayResolutionInput->setValue(omc->delayResolution);
        delayResolutionInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout2, "Delay Resolution (ns):", delayResolutionInput);
        QObject::connect(delayResolutionInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value) {
            omc->delayResolution = value;
            });

        auto pulserDelayInput = new QDoubleSpinBox();
        pulserDelayInput->setRange(0, 2000);
        pulserDelayInput->setValue(omc->pulserBaseDelay);
        pulserDelayInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout2, "Pulser Base Delay:", pulserDelayInput);
        QObject::connect(pulserDelayInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value) {
            omc->pulserBaseDelay = value;
            });

        auto receiverDelayInput = new QDoubleSpinBox();
        receiverDelayInput->setRange(0, 2000);
        receiverDelayInput->setValue(omc->receiverBaseDelay);
        receiverDelayInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout2, "Receiver Base Delay:", receiverDelayInput);
        QObject::connect(receiverDelayInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value) {
            omc->receiverBaseDelay = value;
            });

        tab1Layout->addWidget(group2);
        tab1->setLayout(tab1Layout);

        // Tab 2
        QWidget* tab2 = new QWidget();
        QVBoxLayout* tab2Layout = new QVBoxLayout(tab2);

        // Group 3 - Gate & Ascan Settings
        QGroupBox* group3 = new QGroupBox("Gate & Ascan Settings");
        QVBoxLayout* layout3 = new QVBoxLayout(group3);

        auto ascanStartInput = new QSpinBox();
        ascanStartInput->setRange(0, 20000);
        ascanStartInput->setValue(omc->ascanStart);
        ascanStartInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout3, "Ascan Start Position:", ascanStartInput);
        QObject::connect(ascanStartInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->ascanStart = value;
            });

        auto ascanLengthInput = new QSpinBox();
        ascanLengthInput->setRange(0, 20000);
        ascanLengthInput->setValue(omc->ascanLength);
        ascanLengthInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout3, "Ascan Length:", ascanLengthInput);
        QObject::connect(ascanLengthInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->ascanLength = value;
            });

        auto gateStartInput = new QSpinBox();
        gateStartInput->setRange(0, 20000);
        gateStartInput->setValue(omc->gateStart);
        gateStartInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout3, "Gate Start:", gateStartInput);
        QObject::connect(gateStartInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->gateStart = value;
            });

        auto gateLengthInput = new QSpinBox();
        gateLengthInput->setRange(0, 20000);
        gateLengthInput->setValue(omc->gateLength);
        gateLengthInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout3, "Gate Length:", gateLengthInput);
        QObject::connect(gateLengthInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->gateLength = value;
            });

        auto gateThresholdInput = new QSpinBox();
        gateThresholdInput->setRange(0, 500);
        gateThresholdInput->setValue(omc->gateThreshold);
        gateThresholdInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
        addSetting(layout3, "Gate Threshold:", gateThresholdInput);
        QObject::connect(gateThresholdInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            omc->gateThreshold = value;
            });

        tab2Layout->addWidget(group3);
        tab2->setLayout(tab2Layout);

        // Add tabs to the tab widget
        tabWidget->addTab(tab1, "Settings Tab 1");
        tabWidget->addTab(tab2, "Settings Tab 2");

        QVBoxLayout* mainLayout = new QVBoxLayout(settingsWidget);
        mainLayout->addWidget(tabWidget);
        settingsWidget->setLayout(mainLayout);

        return settingsWidget;
    }

    QWidget* UIManager::createAscanFrame()
    {
        // Create Ascan Frame layout
        QWidget* ascanWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(ascanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto aScanFrm = nFactoryFrame::crAviewFrm();
        nsubject->addObserver(aScanFrm);
        layout->addWidget(aScanFrm->createFrame());
        

        return addFrameName("Ascan", ascanWidget);
    }
    QWidget* UIManager::createSscanFrame()
    {
        QWidget* SscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(SscanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto SviewFrame = nFactoryFrame::crSViewFrm();
        nsubject->addObserver(SviewFrame);
        layout->addWidget(SviewFrame->createFrame());

        return addFrameName("Sscan", SscanWidget);
    }
    QWidget* UIManager::createCscanFrame()
    {
        QWidget* cscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(cscanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto cScanFrm = nFactoryFrame::crCviewFrm();
        nsubject->addObserver(cScanFrm);
        layout->addWidget(cScanFrm->createFrame());

        return addFrameName("Cscan", cscanWidget);
    }
    QWidget* UIManager::createBscanFrame()
    {
        QWidget* Bview = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(Bview);
        layout->setContentsMargins(0, 0, 0, 0);
        //layout->setSpacing(0);

        auto BFrame = nFactoryFrame::crBviewFrm();
        nsubject->addObserver(BFrame);
        layout->addWidget(BFrame->createFrame());

        return addFrameName("Bscan", Bview);
    }
    QWidget* UIManager::create3DFrame() {
        QWidget* VWidget = new QWidget();

        // Initialize Vulkan instance
        QVulkanInstance* inst = new QVulkanInstance();
        inst->setLayers({ "VK_LAYER_KHRONOS_validation" });
        if (!inst->create()) {
            qFatal("Failed to create Vulkan instance");
        }
        
        // Create Vulkan window and set the Vulkan instance
        vulkanWindow = new VulkanWindow;
        vulkanWindow->setVulkanInstance(inst);
        vulkanWindow->GetDeviceInfo();
        // Set the layout to display the window
        QVBoxLayout* layout = new QVBoxLayout(VWidget);
        layout->addWidget(QWidget::createWindowContainer(vulkanWindow));
        layout->setContentsMargins(0, 0, 0, 0);


        return VWidget;
    }

}