#include "UIManager.h"
#include <QApplication>
#include <QSplitter>
#include <QVBoxLayout>

#include "FactoryMgr.h"
#include "Frames/nLogFrame.h"
#include "Frames/CscanFrame.h"
#include "Frames/BscanFrame.h"
#include "Frames/AscanFrame.h"


namespace nmainUI {
    UIManager::UIManager() {
        settings = std::make_unique<QSettings>("RoqView COM", "FrameStatus");
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
    void UIManager::UISETTING(QApplication* app) {
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
        QDialog settingsDialog;
        settingsDialog.setWindowTitle("Settings");
        settingsDialog.setFixedSize(800, 600);

        // Main layout for the dialog
        QVBoxLayout* mainLayout = new QVBoxLayout(&settingsDialog);

        // Create widgets to adjust settings
        QCheckBox* triggerCheckBox = new QCheckBox("Bscan Drag High Res", &settingsDialog);
        triggerCheckBox->setChecked(tsettings->getResolutionBscan());
        mainLayout->addWidget(triggerCheckBox);

        // Horizontal layout for the Apply button
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* applyButton = new QPushButton("Apply", &settingsDialog);
        applyButton->setFixedSize(100, 30);  // Set fixed size for the button
        //buttonLayout->addStretch();  // Push the button to the right
        buttonLayout->addWidget(applyButton);

        // Add the button layout to the main layout
        mainLayout->addLayout(buttonLayout);

        QObject::connect(applyButton, &QPushButton::clicked, [&]() {
            // Apply changes to settings manager
            tsettings->setResolutionBscan(triggerCheckBox->isChecked());
            tsettings->saveToRegistry();
            settingsDialog.accept();
            });

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
        nsubject->addObserver(logFrame);
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
            nsubject->notify();
            });

        // Add a button
        QPushButton* btn = new QPushButton("Load");
        layout->addWidget(btn);
        QObject::connect(btn, &QPushButton::clicked, [=]() mutable {
            app->logical();
            });

        // Add a checkbox
        QCheckBox* checkBox = new QCheckBox("Cscan Layer");
        checkBox->setCheckState(Qt::Unchecked);
        layout->addWidget(checkBox);
        QObject::connect(checkBox, &QCheckBox::stateChanged, [=](int state) {
            isCscanLayer = (state == Qt::Checked);
            nsubject->notify();
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
        // Create Ascan Frame layout
        QWidget* ascanWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(ascanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto aScanFrm = nFactoryFrame::crAscanFrm(app);
        nsubject->addObserver(aScanFrm);
        layout->addWidget(aScanFrm->createFrame());
        

        return addFrameName("Ascan", ascanWidget);
    }
    QWidget* UIManager::createBscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject)
    {
        QWidget* bscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(bscanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto bScanFrm = nFactoryFrame::crBscanFrm(app);
        nsubject->addObserver(bScanFrm);
        layout->addWidget(bScanFrm->createFrame());

        return addFrameName("Bscan", bscanWidget);
    }
    QWidget* UIManager::createCscanFrame(nmainUI::UIFrame* app, shared_ptr<nSubject> nsubject)
    {
        QWidget* cscanWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(cscanWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto cScanFrm = nFactoryFrame::crCscanFrm(app);
        nsubject->addObserver(cScanFrm);
        layout->addWidget(cScanFrm->createFrame());

        return addFrameName("Cscan", cscanWidget);
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
        VulkanWindow* vulkanWindow = new VulkanWindow;
        vulkanWindow->setVulkanInstance(inst);

        // Set the layout to display the window
        QVBoxLayout* layout = new QVBoxLayout(VWidget);
        layout->addWidget(QWidget::createWindowContainer(vulkanWindow));
        layout->setContentsMargins(0, 0, 0, 0);


        return VWidget;
    }

}