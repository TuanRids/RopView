#ifndef OM_SETTING_FRAME_H
#define OM_SETTING_FRAME_H

#include "../pch.h"
#include "SystemConfig/ConfigLocator.h"
class OmSettingFrame : public QWidget {
public:
    static OmSettingFrame *getInstance() {
        static OmSettingFrame Instance;
        static bool isInit = false;
        if (!isInit) {
			Instance.initSettingFrames();
			isInit = true;
		}
		return &Instance;
	}      

private:
    void initSettingFrames() {
        isreal = true;
        auto mainWindow = getMainWindow();
        QDockWidget* dockWidget = new QDockWidget(mainWindow);
        dockWidget->setWindowTitle("Settings");
        dockWidget->setObjectName("SettingsUIManagerDockWidget");
        dockWidget->setMaximumHeight(250);

        settingModeWidget = new QTabWidget();

        QWidget* scanPlanTab = new QWidget();
        QHBoxLayout* scanPlanLayout = new QHBoxLayout(scanPlanTab);
        scanPlanLayout->setContentsMargins(0, 0, 0, 0);
        setupScanPlanTab(scanPlanLayout);
        settingModeWidget->addTab(scanPlanTab, "ScanPlan");

        QWidget* setupConfigTab = new QWidget();
        QHBoxLayout* setupConfigLayout = new QHBoxLayout(setupConfigTab);
        setupConfigLayout->setContentsMargins(0, 0, 0, 0);
        setupConfigTabContent(setupConfigLayout);
        settingModeWidget->addTab(setupConfigTab, "Config");

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(settingModeWidget);

        settingModeWidget->setVisible(true);

        dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
        dockWidget->setWidget(this);
        mainWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget);
        dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    }
    unsigned int cwidth = 300;
    OmSettingFrame() {};
    QSpinBox* createSpinBox(int min, int max, int value, QGridLayout* layout, QString name,int row, int column,  std::function<void(int)> slotFunction ) {
        QSpinBox* spinBox = new QSpinBox();
        spinBox->setRange(min, max);
        spinBox->setValue(value);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setAlignment(Qt::AlignRight);  // Align numbers to the right
        spinBox->setMinimumWidth(80);  // Ensure a consistent width for numbers
        spinBox->setMinimumHeight(10);  // Set minimum height for input box
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), slotFunction);
        addSettingToGrid(layout, name, spinBox,row, column);
        return spinBox;
    }
    QDoubleSpinBox* createDoubleSpinBox(double min, double max, double value, QGridLayout* layout, QString name, int row, int column, std::function<void(double)> slotFunction) {
        QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox();
        doubleSpinBox->setRange(min, max);
        doubleSpinBox->setValue(value);
        doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        doubleSpinBox->setAlignment(Qt::AlignRight);  // Align numbers to the right
        doubleSpinBox->setMinimumWidth(80);  // Ensure a consistent width for numbers
        doubleSpinBox->setMinimumHeight(10);  // Set minimum height for input box
        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), slotFunction);
        addSettingToGrid(layout, name, doubleSpinBox,row, column);
        return doubleSpinBox;
    }

    QTabWidget* settingModeWidget;
    bool isreal;
        
    void setupScanPlanTab(QHBoxLayout* layout) {
        auto scanPlan = OmSetupL::getInstance().OMS;
        
        // Group for Phased Array Transducer Settings
        QGroupBox* group1 = new QGroupBox("Beam Formation");
        QGridLayout* layout1 = new QGridLayout(group1);
        int x = 0; int y = 0;
        {
            createSpinBox(1, 64, scanPlan->beamNumber, layout1, "Beam Quantity", x, y, [=](double value) {
                scanPlan->beamNumber = value;
                });

            createSpinBox(1, 64, scanPlan->EleStep, layout1, "Ele Step", ++x, y, [=](double value) {
                scanPlan->EleStep = value;
                });

            createSpinBox(1, 64, scanPlan->EleFirst, layout1, "Ele First", ++x, y, [=](double value) {
                scanPlan->EleFirst = value;
                });
            x = 0;
            createSpinBox(1, 64, scanPlan->EleLast, layout1, "Ele Last", x, ++y, [=](double value) {
                scanPlan->EleLast = value;
                });

            createSpinBox(1, 64, scanPlan->EleQuantity, layout1, "Ele Quantity", ++x, y, [=](double value) {
                scanPlan->EleQuantity = value;
                });

            createSpinBox(1, 1000, scanPlan->Ele_Delay, layout1, "Ele Delay", ++x, y, [=](double value) {
                scanPlan->Ele_Delay = value;
                });
        }

        // Group 2 - Phased Array Wedge Settings
        QGroupBox* group2 = new QGroupBox("Digitizer");
        QGridLayout* layout2 = new QGridLayout(group2);

        x = 0; y = 0;
        {
            // Ascan Data Size
            QComboBox* ascanDataSizeComboBox = new QComboBox(); auto index = 0;
            ascanDataSizeComboBox->addItem("8 Bits", static_cast<int>(Instrumentation::IAmplitudeSettings::AscanDataSize::EightBits));
            ascanDataSizeComboBox->addItem("12 Bits", static_cast<int>(Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits));
            ascanDataSizeComboBox->addItem("16 Bits", static_cast<int>(Instrumentation::IAmplitudeSettings::AscanDataSize::SixteenBits));
            switch (scanPlan->Digi_Ampli_AscanSize) {
            case Instrumentation::IAmplitudeSettings::AscanDataSize::EightBits: index = 0; break;
            case Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits: index = 1; break;
            case Instrumentation::IAmplitudeSettings::AscanDataSize::SixteenBits: index = 2; break;
            }
            ascanDataSizeComboBox->setCurrentIndex(index);
            layout2->addWidget(new QLabel("Ascan Data Size"), x, y);
            layout2->addWidget(ascanDataSizeComboBox, x, ++y);
            connect(ascanDataSizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                scanPlan->Digi_Ampli_AscanSize = static_cast<Instrumentation::IAmplitudeSettings::AscanDataSize>(ascanDataSizeComboBox->currentData().toInt());
                });


            // Scaling Type
            QComboBox* scalingTypeComboBox = new QComboBox();
            scalingTypeComboBox->addItem("Linear", static_cast<int>(Instrumentation::IAmplitudeSettings::ScalingType::Linear));
            scalingTypeComboBox->addItem("Logarithmic", static_cast<int>(Instrumentation::IAmplitudeSettings::ScalingType::Logarithmic));
            scalingTypeComboBox->setCurrentIndex(static_cast<int>(scanPlan->Digi_Ampli_Scaling));
            layout2->addWidget(new QLabel("Scaling Type"), ++x, --y);
            layout2->addWidget(scalingTypeComboBox, x, ++y);
            connect(scalingTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                scanPlan->Digi_Ampli_Scaling = static_cast<Instrumentation::IAmplitudeSettings::ScalingType>(scalingTypeComboBox->currentData().toInt());
                });

            // Rectification Type
            QComboBox* rectificationTypeComboBox = new QComboBox();
            rectificationTypeComboBox->addItem("None", static_cast<int>(Instrumentation::IAmplitudeSettings::RectificationType::None));
            rectificationTypeComboBox->addItem("Positive", static_cast<int>(Instrumentation::IAmplitudeSettings::RectificationType::Positive));
            rectificationTypeComboBox->addItem("Negative", static_cast<int>(Instrumentation::IAmplitudeSettings::RectificationType::Negative));
            rectificationTypeComboBox->addItem("Full", static_cast<int>(Instrumentation::IAmplitudeSettings::RectificationType::Full));
            rectificationTypeComboBox->setCurrentIndex(static_cast<int>(scanPlan->Digi_Ampli_rectification));
            layout2->addWidget(new QLabel("Rectification Type"), ++x, --y);
            layout2->addWidget(rectificationTypeComboBox, x, ++y);
            connect(rectificationTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                scanPlan->Digi_Ampli_rectification = static_cast<Instrumentation::IAmplitudeSettings::RectificationType>(rectificationTypeComboBox->currentData().toInt());
                nmainUI::statuslogs::getinstance().logInfo("Synchro Mode: " + std::to_string(static_cast<int>(scanPlan->Digi_Ampli_rectification)));
                });

            // Time Compression Factor
            createSpinBox(1, 100, scanPlan->Digi_Time_CompressFactor, layout2, "Time Comp. Factor", ++x, 0, [=](double value) {
                scanPlan->Digi_Time_CompressFactor = static_cast<int>(value);
                });

            x = 0; y = 2;
            // Synchro Mode
            QComboBox* synchroModeComboBox = new QComboBox();
            synchroModeComboBox->addItem("Absolute", static_cast<int>(Instrumentation::ITimeSettings::AscanSynchroMode::Absolute));
            synchroModeComboBox->addItem("RelativeGateSynchro", static_cast<int>(Instrumentation::ITimeSettings::AscanSynchroMode::RelativeGateSynchro));
            // Add other synchro modes here if they exist
            synchroModeComboBox->setCurrentIndex(static_cast<int>(scanPlan->Digi_Time_Synch));
            layout2->addWidget(new QLabel("Synchro Mode"), x, y);
            layout2->addWidget(synchroModeComboBox, x, ++y);
            connect(synchroModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                scanPlan->Digi_Time_Synch = static_cast<Instrumentation::ITimeSettings::AscanSynchroMode>(synchroModeComboBox->currentData().toInt());
                nmainUI::statuslogs::getinstance().logInfo("Synchro Mode: " + std::to_string(static_cast<int>(scanPlan->Digi_Time_Synch)));
                });

            // Decimation Factor
            QComboBox* decimationFactorComboBox = new QComboBox(); auto Deindex = 0;
            decimationFactorComboBox->addItem("One", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::One));
            decimationFactorComboBox->addItem("Two", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::Two));
            decimationFactorComboBox->addItem("Four", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::Four));
            decimationFactorComboBox->addItem("Eight", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::Eight));
            decimationFactorComboBox->addItem("Sixteen", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::Sixteen));
            decimationFactorComboBox->addItem("ThrityTwo", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::ThirtyTwo));
            decimationFactorComboBox->addItem("SixtyFour", static_cast<int>(Instrumentation::ITimeSettings::SamplingDecimationFactor::SixtyFour));
            switch (scanPlan->Digi_Time_DecimaFactor) {
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::One: Deindex = 0; break;
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::Two: Deindex = 1; break;
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::Four: Deindex = 2; break;
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::Eight: Deindex = 3; break;
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::Sixteen: Deindex = 4; break;
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::ThirtyTwo: Deindex = 5; break;
            case Instrumentation::ITimeSettings::SamplingDecimationFactor::SixtyFour: Deindex = 6; break;
            }
            // Add other decimation factors here if they exist
            decimationFactorComboBox->setCurrentIndex(Deindex);
            layout2->addWidget(new QLabel("Decimation Factor"), ++x, --y);
            layout2->addWidget(decimationFactorComboBox, x, ++y);
            connect(decimationFactorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                scanPlan->Digi_Time_DecimaFactor = static_cast<Instrumentation::ITimeSettings::SamplingDecimationFactor>(decimationFactorComboBox->currentData().toInt());
                nmainUI::statuslogs::getinstance().logInfo("Decimation Factor changed to " + std::to_string(static_cast<int>(scanPlan->Digi_Time_DecimaFactor)));
                });

            // Band Pass Filter
            createSpinBox(0, 100, scanPlan->Digi_BandPassFilter, layout2, "Band Pass Filter", 2, 1, [=](double value) {
                scanPlan->Digi_BandPassFilter = static_cast<size_t>(value);
                nmainUI::statuslogs::getinstance().logInfo("Band Pass Filter || TODO Update this instance");

                });
            // Band Pass Filter
            createSpinBox(1, 326, scanPlan->Rate, layout2, "Cycle Rate", 3, 1, [=](double value) {
                scanPlan->Rate = static_cast<size_t>(value);
                });
        }
        
        // Group for Phased Array Transducer Settings
        QGroupBox* group3 = new QGroupBox("Beam Individual");
        QGridLayout* layout3 = new QGridLayout(group3);
        x = 0; y = 0;
        {
            createSpinBox(1, 1000, scanPlan->BeamGain, layout3, "Beam Gain", x, y, [=](double value) {
                scanPlan->BeamGain = value;
                });

            createSpinBox(0, 164800, scanPlan->BeamAStart, layout3, "A Start", ++x, y, [=](double value) {
                scanPlan->BeamAStart = value;
                });

            createSpinBox(0, 164800, scanPlan->BeamAEnd, layout3, "A End", ++x, y, [=](double value) {
                scanPlan->BeamAEnd = value;
                });
            x = 0; y++;

        }
        QGroupBox* group4 = new QGroupBox("Mode");
        QGridLayout* layout4 = new QGridLayout(group4);
        x = 0; y = 0;
        {
            QComboBox* BoxPautMode = new QComboBox(); auto DeModeid = 0;
            BoxPautMode->addItem("Linear", static_cast<int>(PautModeOmni::Linear));
            BoxPautMode->addItem("Sectorial", static_cast<int>(PautModeOmni::Sectorial));
            BoxPautMode->addItem("Sectorial", static_cast<int>(PautModeOmni::Compound));
            BoxPautMode->addItem("Sectorial", static_cast<int>(PautModeOmni::TFM));
            switch (ConfigLocator::getInstance().visualConfig->setPautMode) {
            case PautModeOmni::Linear: DeModeid = 0; break;
            case PautModeOmni::Sectorial: DeModeid = 1; break;
            case PautModeOmni::Compound: DeModeid = 2; break;
            case PautModeOmni::TFM: DeModeid = 3; break;
            }
            BoxPautMode->setCurrentIndex(DeModeid);
            layout4->addWidget(new QLabel("PAUT Mode"), x, y);
            layout4->addWidget(BoxPautMode, x, ++y);
            connect(BoxPautMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                ConfigLocator::getInstance().visualConfig->setPautMode = static_cast<PautModeOmni>(BoxPautMode->currentData().toInt());
                nmainUI::statuslogs::getinstance().logInfo("PAUT Mode changed to " + std::to_string(static_cast<int>(ConfigLocator::getInstance().visualConfig->setPautMode)));
                });
            createSpinBox(0, 100, scanPlan->FocusLength, layout4, "Focus", ++x, --y, [=](double value) {
                scanPlan->FocusLength = value;
                });
            createSpinBox(-90, 90, scanPlan->BeamStartAngle, layout4, "Angel", ++x, y, [=](double value) {
                scanPlan->BeamStartAngle = value;
                });
        }

        QFrame* separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        group1->setFixedWidth(cwidth - 70);
        group2->setFixedWidth(cwidth + 70);
        group3->setFixedWidth(cwidth - 140);
        group4->setFixedWidth(cwidth - 140);

        layout->addWidget(group1);
        layout->addWidget(group2);
        layout->addWidget(group3);
        layout->addWidget(group4);
        layout->addWidget(separator);
    }
    void setupConfigTabContent(QHBoxLayout* layout) {
        auto setupConfig = OmSetupL::getInstance().OMS;
        return;
    }

    void addSettingToGrid(QGridLayout* layout, const QString& label, QWidget* widget, int row, int column) {
        QLabel* labelWidget = new QLabel(label);
        labelWidget->setAlignment(Qt::AlignCenter);

        layout->addWidget(labelWidget, row, column * 2); 
        layout->addWidget(widget, row, column * 2 + 1); 
    }
    void addSetting(QVBoxLayout* layout, const QString& label, QWidget* widget) {
        QHBoxLayout* hLayout = new QHBoxLayout();

        QLabel* labelWidget = new QLabel(label);
        labelWidget->setAlignment(Qt::AlignCenter); 

        hLayout->addWidget(labelWidget);
        hLayout->addWidget(widget);
        layout->addLayout(hLayout);
    }

};

#endif
