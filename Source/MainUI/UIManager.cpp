#include "UIManager.h"

#include "MainUI/OmSettingFrame.h"
//==============Basic Settings ==============
namespace nmainUI {
    UIManager::UIManager() : sttlogs(nullptr), nsubject(nullptr), vulkanWindow(nullptr)
    {
        settings = new QSettings("RoqView COM", "FrameStatus");
        if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    }
    void UIManager::UISETTING() {
        auto gtheme = ConfigLocator::getInstance().settingconf->qsTheme;
        auto sheetName = fs::current_path() / ("QssTemplate\\" + gtheme);
        QString styleSheet;

        QFile file(sheetName);
        if (file.open(QFile::ReadOnly)) {
            styleSheet = QLatin1String(file.readAll());
        }

        qApp->setStyleSheet(styleSheet);

        // Refresh widgets directly inside UISETTING
        for (QWidget* widget : qApp->allWidgets()) {
            widget->setStyleSheet(qApp->styleSheet());
            widget->update();
        }
    }
    void UIManager::saveWidgetState(QWidget* widget) {
        auto mainWindow = getMainWindow();
        settings->setValue("mainWindowState", mainWindow->saveState());
        settings->setValue("mainWindowGeometry", mainWindow->saveGeometry());

        QList<QSplitter*> splitters = widget->findChildren<QSplitter*>();
        for (auto splitter : splitters) {
            settings->setValue(splitter->objectName() + "_splitterState", splitter->saveState());
        }
    }
    void UIManager::restoreWidgetState(QWidget* widget) {
        auto mainWindow = getMainWindow();
        mainWindow->restoreGeometry(settings->value("mainWindowGeometry").toByteArray());
        mainWindow->restoreState(settings->value("mainWindowState").toByteArray());

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
        auto newAction = fileMenu->addAction("New", [=]() { onNewFile(); });
        fileMenu->addAction("Open", [=]() { onOpenFile(); });
        auto saveAction = fileMenu->addAction("Save", [=]() { onSaveFile(); });
        fileMenu->addAction("Load Setup", [=]() { onLoadSetup(); });
        fileMenu->addAction("Save Setup", [=]() { onSaveSetup(); });
        fileMenu->addAction("Setting", [=]() { openSettingsDialog(); });
        auto exitAction = fileMenu->addAction("Exit", [=]() { onExit(); });

        /*std::unique_ptr<IData3DProcecss> object = std::make_unique<Data3DProcess>();
            auto result = object->processData();                                   TODO ///////////////////////3D data process///////////////////////
            vulkanWindow->PautINIT(result);*/

        auto cutAction = editMenu->addAction("Cut", [=]() { onCut(); });
        auto copyAction = editMenu->addAction("Copy", [=]() { onCopy(); });
        auto pasteAction = editMenu->addAction("Paste", [=]() { onPaste(); });

        auto fullScreenAction = viewMenu->addAction("FullScreen", [=]() { onFullScreen(); });
        viewMenu->addAction("Debug Logs", [=]() { showRealTimeLogs(); });
        showRealTimeLogs();

        newAction->setEnabled(false);
		saveAction->setEnabled(false);
		cutAction->setEnabled(false);
		copyAction->setEnabled(false);
		pasteAction->setEnabled(false);
		fullScreenAction->setEnabled(false);

        // Add the QMenuBar to the layout
        layout->setMenuBar(menuBar);

        return menuBarWidget;
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

//============= Other Widgets & Dialogs =============
    void UIManager::showRealTimeLogs() {
        auto nmainwd = getMainWindow();
        QDockWidget* logDockWidget = new QDockWidget("Real-Time Logs", nmainwd);
        logDockWidget->setObjectName("RealTimeLogsUIManagerDockWidget");
        QTextEdit* logTextEdit = new QTextEdit(logDockWidget);
        logTextEdit->setReadOnly(true);

        QFont monospaceFont("Courier New");
        monospaceFont.setStyleHint(QFont::Monospace);
        logTextEdit->setFont(monospaceFont);

        logTextEdit->setWordWrapMode(QTextOption::NoWrap);

        logDockWidget->setWidget(logTextEdit);
        nmainwd->addDockWidget(Qt::RightDockWidgetArea, logDockWidget);

        QString logFilePath = QString::fromStdString(ConfigLocator::getInstance().sysParams->tempBufferLogs);
        static qint64 lastPos = 0;

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
                    logTextEdit->moveCursor(QTextCursor::End);
                    logTextEdit->insertPlainText(newContent);
                    logTextEdit->moveCursor(QTextCursor::End);
                }
            }
            });

        logUpdateTimer->start(1000);
        logDockWidget->show();

    }
    void UIManager::openSettingsDialog() {
        auto currentSettings = ConfigLocator::getInstance().settingconf;

        QDialog settingsDialog;
        settingsDialog.setWindowTitle("Settings");

        QHBoxLayout* mainLayout = new QHBoxLayout(&settingsDialog);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* applyButton = new QPushButton("Apply", &settingsDialog);
        applyButton->setFixedSize(100, 30);

        // Group Box to hold all settings with fixed size
        {
            QGroupBox* settingsGroup = new QGroupBox("Omniscan Settings", &settingsDialog);
            settingsGroup->setFixedSize(200, 200);
            QFormLayout* formLayout = new QFormLayout(settingsGroup);

            // Resolution setting
            auto resInput = new QSpinBox();
            resInput->setRange(1, 100);
            resInput->setValue(ConfigLocator::getInstance().sysParams->resolution);
            formLayout->addRow(new QLabel("Res:"), resInput);

            QObject::connect(resInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) mutable {
                ConfigLocator::getInstance().sysParams->resolution = resInput->value();
                nsubject->stopRealtimeUpdate();
                });

            // Checkbox Cscan Layer
            QCheckBox* checkBox = new QCheckBox("Cscan Layer");
            checkBox->setCheckState(ConfigLocator::getInstance().sysParams->isCscanLayer ? Qt::Checked : Qt::Unchecked);
            formLayout->addRow(checkBox);

            QObject::connect(checkBox, &QCheckBox::stateChanged, [=](int state) {
                ConfigLocator::getInstance().sysParams->isCscanLayer = (state == Qt::Checked);
                });

            // Existing settings
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
        mainLayout->addWidget(applyButton);

        QObject::connect(applyButton, &QPushButton::clicked, [&]() {
            ConfigLocator::getInstance().saveToRegistry();
            UISETTING();
            sttlogs->logInfo("Settings applied.");
            settingsDialog.accept();
            });

        settingsDialog.exec();
    }


//============== Create Settings Frame Widgets ==============

    QWidget* UIManager::createLogFrame() {
        QWidget* logWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(logWidget);
        layout->setContentsMargins(0, 0, 0, 0);

        QTextEdit* logOutput = new QTextEdit();
        logOutput->setReadOnly(true);

        nmainUI::statuslogs::getinstance().initialize(logOutput);

        layout->addWidget(logOutput);
        return logWidget;
    }
    QWidget* UIManager::createConnectFrames() {
        QWidget* settingsWidget = new QWidget();

        QVBoxLayout* layout = new QVBoxLayout(settingsWidget);
        layout->setContentsMargins(1, 1, 1, 1);

        QPushButton* StartOmni = new QPushButton("Start");
        layout->addWidget(StartOmni);
                
        // Create a button for STOP action
        QPushButton* stopButton = new QPushButton("Stop");
        layout->addWidget(stopButton);

        QObject::connect(StartOmni, &QPushButton::clicked, [=]() mutable {
            nsubject->stopNotifyTimer();
            if (IOmConnect::Create()->omConnectDevice(ConnectMode::SetupFileMode)) {
                sttlogs->logNotify("Start connecting to the device!");
                StartOmni->setText("Update");
            }
            else {
                IOmConnect::ReleaseDevice();
                sttlogs->logWarning("Release device! Check your configuration.");
                StartOmni->setText("Start");
            }
            nsubject->startRealtimeUpdate(45);
            });

        QObject::connect(stopButton, &QPushButton::clicked, [=]() mutable {
            nsubject->stopRealtimeUpdate();
            IOmConnect::ReleaseDevice();
            sttlogs->logNotify("Stop!");
            StartOmni->setText("Start");
            });

        return settingsWidget;
    }
    void UIManager::createLogWidget() {
        auto nmainwd = getMainWindow();
        QDockWidget* logWidget = new QDockWidget(nmainwd);
        logWidget->setObjectName("logBottomUIManagerDockWidget");
        logWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

        logWidget->setTitleBarWidget(new QWidget());

        QWidget* logContainer = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(logContainer);
        layout->setContentsMargins(0, 0, 0, 0);

        QTextEdit* logOutput1 = new QTextEdit();
        logOutput1->setLineWrapMode(QTextEdit::NoWrap);
        logOutput1->setReadOnly(true);
        logOutput1->setFixedHeight(25);  

        QTextEdit* logOutput11 = new QTextEdit();
        logOutput11->setLineWrapMode(QTextEdit::NoWrap);
        logOutput11->setReadOnly(true);
        logOutput11->setFixedHeight(25);

        QTextEdit* logOutput2 = new QTextEdit();
        logOutput2->setLineWrapMode(QTextEdit::NoWrap);
        logOutput2->setReadOnly(true);
        logOutput2->setFixedHeight(25);  

        QTextEdit* logOutput3 = new QTextEdit();
        logOutput3->setLineWrapMode(QTextEdit::NoWrap);
        logOutput3->setReadOnly(true);
        logOutput3->setFixedHeight(25); 

        logOutput1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        logOutput1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        logOutput2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        logOutput2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        logOutput3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        logOutput3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        layout->addWidget(logOutput1);
        layout->addWidget(logOutput11);
        layout->addWidget(logOutput2);
        layout->addWidget(logOutput3);

        logWidget->setWidget(logContainer);

        QTimer* timer = new QTimer(logWidget);
        QObject::connect(timer, &QTimer::timeout, [logOutput1, logOutput11, logOutput2, logOutput3]() {
            static auto readPAUTstatus = &recordReadingPAUT::getinstance();
            static auto processdataStatus = &recordProcessingData::getinstance();
            size_t buffer = std::make_shared<upFrame>()->bufferSize();

            logOutput1->clear();
            QString text1 = QString("Buffer Size: %1").arg(static_cast<int>(buffer));
            logOutput1->append(text1);

            logOutput2->clear();
            QString text2 = QString("Throughput Mbs: %1").arg(readPAUTstatus->get(), 0, 'f', 2);
            logOutput2->append(text2);

            logOutput11->clear();
            QString text11 = QString("Read FPS Size: %1").arg(readPAUTstatus->get_FPS(), 0, 'f', 2);
            logOutput11->append(text11);

            logOutput3->clear();
            QString text3 = QString("Frame Time ms: %1").arg(processdataStatus->get_FPS(), 0, 'f', 2);
            logOutput3->append(text3);

            });

        timer->start(100);

        logWidget->show();

        nmainwd->addDockWidget(Qt::BottomDockWidgetArea, logWidget);
    }   
//============== Create Frame Widgets ==============

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
        inst->setApiVersion(QVersionNumber::fromString("1.2"));


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

//============== Internal MenuBar Functions ==============
    void UIManager::onOpenFile()
    {
        nsubject->stopRealtimeUpdate();
        nsubject->startNotifyTimer(52);
        IOmConnect::ReleaseDevice();
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
    void nmainUI::UIManager::onLoadSetup()
    {
    }
    void nmainUI::UIManager::onSaveSetup()
    {

    }

}