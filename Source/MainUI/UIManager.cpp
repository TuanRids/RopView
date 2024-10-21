#include "UIManager.h"
#include <QApplication>
#include <QSplitter>
#include <QVBoxLayout>

#include "FactoryMgr.h"
#include "Frames/nLogFrame.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/BviewFrame.h"
#include "Frames/AviewFrame.h"

#include "ObserverMgr.h"
#include "..\Source\PAUTFileReader\AscanProcessor.h"

#include "..\Model3D\MainViewport.h"

#include "..\OmConnect\OmConnect.h"
#include "..\Data3DProcessing\DataProcess.h"
namespace nmainUI {
    UIManager::UIManager() {
        settings = std::make_unique<QSettings>("RoqView COM", "FrameStatus");
        if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    }
    QWidget* UIManager::createMenuBarFrame(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject)
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


        // Add the QMenuBar to the layout
        layout->setMenuBar(menuBar);

        return menuBarWidget;
    }
    void UIManager::refreshWidgets(const QList<QWidget*>& widgets) {
        for (QWidget* widget : widgets) {
            widget->setStyleSheet(qApp->styleSheet());
            widget->update();
        }
    }

    void UIManager::UISETTING() {
        auto gtheme = SettingsManager::getInstance()->getSettings().qsTheme;
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
    void UIManager::openSettingsDialog()
    {
        SettingsManager* tsettings = SettingsManager::getInstance();
        SettingParams currentSettings = tsettings->getSettings();

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
            triggerCheckBox->setChecked(currentSettings.bhighResBscan);
            formLayout->addRow(triggerCheckBox);

            QLineEdit* ipAddressField = new QLineEdit(QString::fromStdString(currentSettings.ipAddress), settingsGroup);
            ipAddressField->setToolTip("Enter the IP Address");
            formLayout->addRow(new QLabel("IP Address:"), ipAddressField);

            QSpinBox* portSpinBox = new QSpinBox(settingsGroup);
            portSpinBox->setRange(1, 65535);
            portSpinBox->setValue(currentSettings.port);
            portSpinBox->setToolTip("Set the Port number");
            formLayout->addRow(new QLabel("Port:"), portSpinBox);

            QSpinBox* timeoutSpinBox = new QSpinBox(settingsGroup);
            timeoutSpinBox->setRange(1000, 60000);
            timeoutSpinBox->setValue(currentSettings.timeout);
            timeoutSpinBox->setToolTip("Set the timeout in milliseconds");
            formLayout->addRow(new QLabel("Timeout (ms):"), timeoutSpinBox);

            QLineEdit* deviceNameField = new QLineEdit(QString::fromStdString(currentSettings.device_name), settingsGroup);
            deviceNameField->setToolTip("Enter the device name");
            formLayout->addRow(new QLabel("Device Name:"), deviceNameField);

            // Add the group box to the main layout
            mainLayout->addWidget(settingsGroup);

            QObject::connect(applyButton, &QPushButton::clicked, [&]() {
                currentSettings.bhighResBscan = triggerCheckBox->isChecked();
                currentSettings.ipAddress = ipAddressField->text().toStdString();
                currentSettings.port = portSpinBox->value();
                currentSettings.timeout = timeoutSpinBox->value();
                currentSettings.device_name = deviceNameField->text().toStdString();

                tsettings->updateSettings(currentSettings);
                tsettings->saveToRegistry();
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
            themeComboBox->setCurrentText(QString::fromStdString(currentSettings.qsTheme));

            formLayout->addRow(new QLabel("Theme:"), themeComboBox);
            
            QPushButton* reloadTheme = new QPushButton("Hotload");
            formLayout->addWidget(reloadTheme);
            QObject::connect(reloadTheme, &QPushButton::clicked, [&]() {
                currentSettings.qsTheme = themeComboBox->currentText().toStdString();
                tsettings->updateSettings(currentSettings);
                UISETTING();
                sttlogs->logInfo("Reload to use theme: " + currentSettings.qsTheme);
                });

            mainLayout->addWidget(settingsGroup);

            QObject::connect(applyButton, &QPushButton::clicked, [&]() {
                tsettings->saveToRegistry();
                UISETTING();
                sttlogs->logInfo("Settings applied.");
                });
            
        }


        mainLayout->addLayout(buttonLayout);

        settingsDialog.exec();
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
    QWidget* UIManager::createLogFrame(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject) {
        QWidget* logWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(logWidget);
        layout->setContentsMargins(0, 0, 0, 0);

        auto logFrame = nFactoryFrame::crLogFrm();
        nsubject->addOfflineObserver(logFrame);
        layout->addWidget(logFrame->createFrame());

        return logWidget;

    }
    QWidget* UIManager::createLogSettings(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject) {
        QWidget* settingsWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(settingsWidget);
        layout->setContentsMargins(1, 1, 1, 1);

        // Resolution setting
        auto resInput = new QSpinBox();
        resInput->setRange(1, 100);
        resInput->setValue(resolution);
        auto resLayout = new QHBoxLayout();
        resLayout->addWidget(new QLabel("Res:"));
        resLayout->addWidget(resInput);
        layout->addLayout(resLayout);

        QObject::connect(resInput, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) mutable {
            resolution = resInput->value();
            nsubject->notify(nullptr);
            });

        // Add a button
        QPushButton* btnLoad = new QPushButton("Load");
        layout->addWidget(btnLoad);
        QObject::connect(btnLoad, &QPushButton::clicked, [=]() mutable {
            app->logical();
            });

        // Add a button
        QPushButton* btnConnect = new QPushButton("Connect");
        layout->addWidget(btnConnect);
        QObject::connect(btnConnect, &QPushButton::clicked, [=]() mutable {
            omc->omConnectDevice();
            // C:/Users/ngdtu/source/repos/RopView/logs.ps1
            nsubject->startRealtimeUpdate(10);

            });
        // add 3D button
		QPushButton* btn3D = new QPushButton("3D");
		layout->addWidget(btn3D);
        QObject::connect(btn3D, &QPushButton::clicked, [=]() mutable {
            std::unique_ptr<IDataProcecss> object = std::make_unique<DataProcess>();
            auto result = object->processData();
            vulkanWindow->PautINIT(result);
            });
         
        
        // Add a checkbox
        QCheckBox* checkBox = new QCheckBox("Cscan Layer");
        checkBox->setCheckState(Qt::Unchecked);
        layout->addWidget(checkBox);
        QObject::connect(checkBox, &QCheckBox::stateChanged, [=](int state) {
            isCscanLayer = (state == Qt::Checked);
            nsubject->notify(nullptr);
            });

        return settingsWidget;
    }
    QWidget* UIManager::createSetting2(nmainUI::UIFrame* app, std::shared_ptr<nSubject> nsubject)
    {
        QWidget* logWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(logWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        return logWidget;
    }            
    QWidget* UIManager::createAscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject)
    {
        QWidget* ascanWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(ascanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto offlineFrame = nFactoryFrame::crAviewFrm(app);   // Frame offline
        auto realTimeFrame = nFactoryFrame::crAvRealFrm(app); // Frame real-time

        nsubject->addOfflineObserver(offlineFrame);
        nsubject->addRealtimeObserver(realTimeFrame);

        layout->addWidget(offlineFrame->createFrame());

        return addFrameName("Ascan", ascanWidget);
    }
    QWidget* UIManager::createSscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject)
    {
        QWidget* SscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(SscanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto SviewFrame = nFactoryFrame::crSViewFrm(app);
        nsubject->addOfflineObserver(SviewFrame);
        layout->addWidget(SviewFrame->createFrame());

        return addFrameName("Sscan", SscanWidget);
    }
    QWidget* UIManager::createCscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject)
    {
        QWidget* cscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(cscanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto cScanFrm = nFactoryFrame::crCviewFrm(app);
        nsubject->addOfflineObserver(cScanFrm);
        layout->addWidget(cScanFrm->createFrame());

        return addFrameName("Cscan", cscanWidget);
    }
    QWidget* UIManager::createBscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject)
    {
        QWidget* Bview = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(Bview);
        layout->setContentsMargins(0, 0, 0, 0);
        //layout->setSpacing(0);

        auto BFrame = nFactoryFrame::crBviewFrm(app);
        nsubject->addOfflineObserver(BFrame);
        layout->addWidget(BFrame->createFrame());

        return addFrameName("Bscan", Bview);
    }
    QWidget* UIManager::create3DFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject) {

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