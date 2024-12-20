#include "UIManager.h"

#include "MainUI/OmSettingFrame.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"
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

        QObject::connect(logDockWidget, &QDockWidget::visibilityChanged, [logUpdateTimer](bool visible) {
            if (visible) {
                logUpdateTimer->start(1000);
            }
            else {
                logUpdateTimer->stop();
                lastPos = 0;
            }
            });

        logDockWidget->show();

    }
    void UIManager::openSettingsDialog() {
        auto currentSettings = ConfigLocator::getInstance().settingconf;

        QDialog settingsDialog;
        settingsDialog.setWindowTitle("Other Settings");

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
    int MarginSize = 3;
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
            nsubject->stopRealtimeUpdate();
            nsubject->stopNotifyTimer();
            if (IOmConnect::Create()->omConnectDevice(ConnectMode::SetupFileMode)) {
                sttlogs->logNotify("Start connecting to the device!");
                StartOmni->setText("Update");
            }
            else {
                sttlogs->logWarning("Check your configuration.");
                // StartOmni->setText("Start");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            nsubject->startRealtimeUpdate();
            });

        QObject::connect(stopButton, &QPushButton::clicked, [=]() mutable {
            nsubject->stopRealtimeUpdate();
            nsubject->stopNotifyTimer();
            IOmConnect::ReleaseDevice();
            sttlogs->logNotify("Release device!");
            StartOmni->setText("Start");
            });

        return settingsWidget;
    }
    void UIManager::createLogWidget() {
        auto nmainwd = getMainWindow();
        QDockWidget* logWidget = new QDockWidget("Processing Status", nmainwd);
        logWidget->setObjectName("logBottomUIManagerDockWidget");

        QWidget* logContainer = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(logContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        size_t table_size = 5;
        QTableWidget* logTable = new QTableWidget(table_size, 2, logContainer);
        logTable->horizontalHeader()->setVisible(false);
        logTable->verticalHeader()->setVisible(false);
        logTable->setSelectionMode(QAbstractItemView::NoSelection);
        logTable->setFocusPolicy(Qt::NoFocus);
        logTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        logTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);



        logTable->setItem(0, 0, new QTableWidgetItem("BS"));
        logTable->item(0, 0)->setFlags(logTable->item(0, 0)->flags() & ~Qt::ItemIsEditable);
        logTable->setItem(1, 0, new QTableWidgetItem("TPS (MB/s)"));
        logTable->item(1, 0)->setFlags(logTable->item(1, 0)->flags() & ~Qt::ItemIsEditable);
        logTable->setItem(2, 0, new QTableWidgetItem("(T1) R.PAUT (ms)"));
        logTable->item(2, 0)->setFlags(logTable->item(2, 0)->flags() & ~Qt::ItemIsEditable);
        logTable->setItem(3, 0, new QTableWidgetItem("(T2) Proc (ms)"));
        logTable->item(3, 0)->setFlags(logTable->item(3, 0)->flags() & ~Qt::ItemIsEditable);
        logTable->setItem(4, 0, new QTableWidgetItem("(GPU) Sview (ms)"));
        logTable->item(4, 0)->setFlags(logTable->item(4, 0)->flags() & ~Qt::ItemIsEditable);

        for (int i = 0; i < table_size; ++i) {
            logTable->setItem(i, 1, new QTableWidgetItem("-"));
            logTable->item(i, 1)->setFlags(logTable->item(i, 1)->flags() & ~Qt::ItemIsEditable);
        }


        logTable->item(0, 0)->setToolTip("Buffer Size");
        logTable->item(1, 0)->setToolTip("Throughtput per second (MB/s)");
        logTable->item(2, 0)->setToolTip("Time spent reading PAUT data (ms)");
        logTable->item(3, 0)->setToolTip("Processing time in milliseconds (only when data exists)");
        logTable->item(4, 0)->setToolTip("GPU rendering time for Sview in milliseconds");

        for (int i = 0; i < table_size; ++i) {
            logTable->setItem(i, 1, new QTableWidgetItem("-"));
            logTable->item(i, 1)->setFlags(logTable->item(i, 1)->flags() & ~Qt::ItemIsEditable);
        }

        layout->addWidget(logTable);
        logWidget->setWidget(logContainer);
        QTimer* timer = new QTimer(logWidget);
        QObject::connect(timer, &QTimer::timeout, [logTable]() {
            static std::mutex mtx;
            static PAUTManager* pautmgr = &PAUTManager::getInstance();
            static auto readStatus = &ReadStatus::getinstance();
            size_t buffer = pautmgr->bufferSize();

            logTable->item(0, 1)->setText(QString::number(static_cast<int>(buffer)));
            logTable->item(1, 1)->setText(QString::number(readStatus->get_throughout(), 'f', 2));
            logTable->item(2, 1)->setText(QString::number(readStatus->get_readPAUT(), 'f', 2));
            logTable->item(3, 1)->setText(QString::number(readStatus->get_processData(), 'f', 2));
            logTable->item(4, 1)->setText(QString::number(readStatus->get_sviewfps(), 'f', 2));
            //table_size
            });

        timer->start(1000);
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
        label->setGeometry(0, -5, 100, 20);

        label->setAttribute(Qt::WA_TransparentForMouseEvents);
        label->raise();
        return frame;
    }
    QWidget* UIManager::createAscanFrame()
    {
        // Create Ascan Frame layout
        QWidget* ascanWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(ascanWidget);
        layout->setContentsMargins(MarginSize, MarginSize, MarginSize, MarginSize);
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
        layout->setContentsMargins(MarginSize, MarginSize, MarginSize, MarginSize);
        layout->setSpacing(0);
               
        auto sViewFrm = nFactoryFrame::crSViewFrm(SscanWidget);
        nsubject->addObserver(sViewFrm);
        layout->addWidget(sViewFrm->createFrame());


        return addFrameName("Sscan", SscanWidget);
    }
    QWidget* UIManager::createCscanFrame()
    {
        QWidget* cscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(cscanWidget);
        layout->setContentsMargins(MarginSize, MarginSize, MarginSize, MarginSize);
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
        layout->setContentsMargins(MarginSize, MarginSize, MarginSize, MarginSize);
        //layout->setSpacing(0);

        auto BFrame = nFactoryFrame::crBviewFrm();
        nsubject->addObserver(BFrame);
        layout->addWidget(BFrame->createFrame());

        return addFrameName("Bscan", Bview);
    }
    QWidget* UIManager::create3DFrame() {
        QWidget* VWidget = new QWidget();
        
        // Initialize Vulkan instance
        //QVulkanInstance* inst = new QVulkanInstance();
        //inst->setApiVersion(QVersionNumber::fromString("1.2"));
        //
        //
        //inst->setLayers({ "VK_LAYER_KHRONOS_validation" });
        //if (!inst->create()) {
        //    qFatal("Failed to create Vulkan instance");
        //}
        //
        //// Create Vulkan window and set the Vulkan instance
        //vulkanWindow = new VulkanWindow;
        //vulkanWindow->setVulkanInstance(inst);
        //vulkanWindow->GetDeviceInfo();
        // // Set the layout to display the window
        //QVBoxLayout* layout = new QVBoxLayout(VWidget);
        //layout->addWidget(QWidget::createWindowContainer(vulkanWindow));
        //layout->setContentsMargins(0, 0, 0, 0);
        //
        return VWidget;
    }

//============== Internal MenuBar Functions ==============
    void UIManager::onOpenFile()
    {
        IOmConnect::ReleaseDevice();
        sttlogs->logNotify("Stop!");
        if (StartOmni) StartOmni->setText("Start");
        nsubject->startNotifyTimer(52);
        IOmConnect::ReleaseDevice();
        if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
        static std::shared_ptr<AscanProcessor> processor;
        if (!processor) { processor = std::make_shared<AscanProcessor>(); }
        if (processor)
        {
            PAUTManager* pautmgr = &PAUTManager::getInstance();
            pautmgr->clearAll();

            auto factframe = nFactoryFrame::crCviewFrm();
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