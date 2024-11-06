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
    unsigned int cwidth = 220;
    OmSettingFrame() {};
    QSpinBox* createSpinBox(int min, int max, int value, std::function<void(int)> slotFunction) {
        QSpinBox* spinBox = new QSpinBox();
        spinBox->setRange(min, max);
        spinBox->setValue(value);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setAlignment(Qt::AlignRight);  // Align numbers to the right
        spinBox->setMinimumWidth(80);  // Ensure a consistent width for numbers
        spinBox->setMinimumHeight(10);  // Set minimum height for input box
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), slotFunction);
        return spinBox;
    }
    QDoubleSpinBox* createDoubleSpinBox(double min, double max, double value, std::function<void(double)> slotFunction) {
        QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox();
        doubleSpinBox->setRange(min, max);
        doubleSpinBox->setValue(value);
        doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        doubleSpinBox->setAlignment(Qt::AlignRight);  // Align numbers to the right
        doubleSpinBox->setMinimumWidth(80);  // Ensure a consistent width for numbers
        doubleSpinBox->setMinimumHeight(10);  // Set minimum height for input box
        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), slotFunction);
        return doubleSpinBox;
    }

    QTabWidget* settingModeWidget;
    bool isreal;
        
    void setupScanPlanTab(QHBoxLayout* layout) {
        auto scanPlan = OmSetupL::getInstance().OMS;
        
        // Group for Phased Array Transducer Settings
        QGroupBox* group1 = new QGroupBox("Scan Plan");
        QVBoxLayout* layout1 = new QVBoxLayout(group1);

        auto transducerFrequencyInput = createDoubleSpinBox(1.0, 15.0, scanPlan->raProbeFre, [=](double value) {
            scanPlan->raProbeFre = value;
            });
        addSetting(layout1, "Transducer Frequency", transducerFrequencyInput);

        auto elementQtyInput = createSpinBox(1, 128, scanPlan->raProbeElem, [=](int value) {
            scanPlan->raProbeElem = value;
            });
        addSetting(layout1, "Element Quantity", elementQtyInput);

        auto rowQtyInput = createSpinBox(1, 10, scanPlan->raProbeRows, [=](int value) {
            scanPlan->raProbeRows = value;
            });

        addSetting(layout1, "Row Quantity", rowQtyInput);

        auto widthInput = createDoubleSpinBox(0.1, 5.0, scanPlan->raProbeWidth, [=](double value) {
            scanPlan->raProbeWidth = value;
            });
        addSetting(layout1, "Width (mm)", widthInput);

        auto heightInput = createDoubleSpinBox(1.0, 20.0, scanPlan->raProbeHeight, [=](double value) {
            scanPlan->raProbeHeight = value;
            });
        addSetting(layout1, "Height (mm)", heightInput);

        // Group 2 - Phased Array Wedge Settings
        QGroupBox* group2 = new QGroupBox("Wedge Settings");
        QVBoxLayout* layout2 = new QVBoxLayout(group2);

        auto wedgeLengthInput = createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeLength, [=](double value) {
            scanPlan->raWedgeLength = value;
            });
        addSetting(layout2, "Length (mm)", wedgeLengthInput);

        auto wedgeWidthInput = createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeWidth, [=](double value) {
            scanPlan->raWedgeWidth = value;
            });
        addSetting(layout2, "Width (mm)", wedgeWidthInput);

        auto wedgeHeightInput = createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeHeight, [=](double value) {
            scanPlan->raWedgeHeight = value;
            });
        addSetting(layout2, "Height (mm)", wedgeHeightInput);

        auto wedgeExitPointInput = createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeExitPoint, [=](double value) {
            scanPlan->raWedgeExitPoint = value;
            });
        addSetting(layout2, "Exit Point (mm)", wedgeExitPointInput);

        auto wedgeMaterialVelocityInput = createDoubleSpinBox(1000.0, 20000.0, scanPlan->raWedgeMaterialVelocity, [=](double value) {
            scanPlan->raWedgeMaterialVelocity = value;
            });
        addSetting(layout2, "Material Velocity (m/s)", wedgeMaterialVelocityInput);

        // Group 3 - Phased Array Linear Formation
        QGroupBox* group3 = new QGroupBox("Linear Formation");
        QVBoxLayout* layout3 = new QVBoxLayout(group3);

        auto elementStepInput = createSpinBox(1, 32, scanPlan->EleStep, [=](double value) {
            scanPlan->EleStep = value;
            });
        addSetting(layout3, "Step", elementStepInput);

        auto activeElementsInput = createSpinBox(1, 32, scanPlan->EleQuantity, [=](int value) {
            scanPlan->EleQuantity = value;
            });
        addSetting(layout3, "Quantity", activeElementsInput);

        auto startElementInput = createSpinBox(1, 32, scanPlan->EleFirst, [=](int value) {
            scanPlan->EleFirst = value;
            });
        addSetting(layout3, "First", startElementInput);

        auto maxElementsInput = createSpinBox(1, 64, scanPlan->EleLast, [=](int value) {
            scanPlan->EleLast = value;
            });
        addSetting(layout3, "Last", maxElementsInput);

        // Group 4 - Phased Array Beam Settings
        QGroupBox* group4 = new QGroupBox("Beam Settings");
        QGridLayout* layout4 = new QGridLayout(group4);

        auto focusingDepthInput = createDoubleSpinBox(1.0, 200.0, scanPlan->beamFocusingDepth, [=](double value) {
            scanPlan->beamFocusingDepth = value;
            });
        addSettingToGrid(layout4, "Focusing Depth (mm)", focusingDepthInput, 0, 0);

        auto refractedAngleInput = createDoubleSpinBox(0.0, 90.0, scanPlan->beamRefractedAngle, [=](double value) {
            scanPlan->beamRefractedAngle = value;
            });
        addSettingToGrid(layout4, "Refracted Angle (deg)", refractedAngleInput, 0, 1);

        auto probePositionAngleInput = createDoubleSpinBox(0.0, 360.0, scanPlan->probePositionAngle, [=](double value) {
            scanPlan->probePositionAngle = value;
            });
        addSettingToGrid(layout4, "Probe pos Angle (deg)", probePositionAngleInput, 1, 0);

        auto probePositionXInput = createDoubleSpinBox(-100.0, 100.0, scanPlan->probePositionX, [=](double value) {
            scanPlan->probePositionX = value;
            });
        addSettingToGrid(layout4, "Probe pos X (mm)", probePositionXInput, 1, 1);

        auto probePositionYInput = createDoubleSpinBox(-100.0, 100.0, scanPlan->probePositionY, [=](double value) {
            scanPlan->probePositionY = value;
            });
        addSettingToGrid(layout4, "Probe pos Y (mm)", probePositionYInput, 2, 0);

        auto skewStartInput = createDoubleSpinBox(0.0, 90.0, scanPlan->skewStart, [=](double value) {
            scanPlan->skewStart = value;
            });
        addSettingToGrid(layout4, "Skew Start (deg)", skewStartInput, 2, 1);

        auto skewStopInput = createDoubleSpinBox(0.0, 90.0, scanPlan->skewStop, [=](double value) {
            scanPlan->skewStop = value;
            });
        addSettingToGrid(layout4, "Skew Stop (deg)", skewStopInput, 3, 0);

        auto skewStepInput = createDoubleSpinBox(0.1, 10.0, scanPlan->skewStep, [=](double value) {
            scanPlan->skewStep = value;
            });
        addSettingToGrid(layout4, "Skew Step (deg)", skewStepInput, 3, 1);

        QFrame* separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        group1->setFixedWidth(cwidth);
        group2->setFixedWidth(cwidth);
        group3->setFixedWidth(cwidth);
        group4->setFixedWidth(cwidth*2);

        layout->addWidget(group1);
        layout->addWidget(group2);
        layout->addWidget(group3);
        layout->addWidget(group4);
        layout->addWidget(separator);
    }
    void setupConfigTabContent(QHBoxLayout* layout) {
        auto setupConfig = OmSetupL::getInstance().OMS;

        // Group for Phasing Settings
        QGroupBox* group1 = new QGroupBox("Phasing Settings");
        QGridLayout* layout1 = new QGridLayout(group1);


        auto gainInput = createDoubleSpinBox(0.0, 200., setupConfig->phasing_gain, [=](double value) {
            setupConfig->phasing_gain = value;
            });
        addSettingToGrid(layout1, "Gain", gainInput, 0, 0);

        auto velocityInput = createDoubleSpinBox(1000.0, 20000.0, setupConfig->phasing_velocity, [=](double value) {
            setupConfig->phasing_velocity = value;
            });
        addSettingToGrid(layout1, "Velocity (m/s)", velocityInput, 0, 1);

        auto referenceAmplitudeInput = createDoubleSpinBox(0.0, 100.0, setupConfig->phasing_referenceAmplitude, [=](double value) {
            setupConfig->phasing_referenceAmplitude = value;
            });
        addSettingToGrid(layout1, "Reference Amplitude", referenceAmplitudeInput, 1, 0);

        auto pulseWidthInput = createSpinBox(10, 500, setupConfig->phasing_pulseWidth, [=](int value) {
            setupConfig->phasing_pulseWidth = value;
            });
        addSettingToGrid(layout1, "Pulse Width (ns)", pulseWidthInput, 1, 1);

        // Dropdown for Ascan Data Size
        QComboBox* ascanDataSizeDropdown = new QComboBox();
        ascanDataSizeDropdown->addItem("8 Bits", QVariant::fromValue(Instrumentation::IAmplitudeSettings::AscanDataSize::EightBits));
        ascanDataSizeDropdown->addItem("12 Bits", QVariant::fromValue(Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits));
        ascanDataSizeDropdown->addItem("16 Bits", QVariant::fromValue(Instrumentation::IAmplitudeSettings::AscanDataSize::SixteenBits));
        ascanDataSizeDropdown->setCurrentIndex(static_cast<int>(Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits));

        connect(ascanDataSizeDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            setupConfig->phasing_ascanDataSize = static_cast<Instrumentation::IAmplitudeSettings::AscanDataSize>(ascanDataSizeDropdown->itemData(index).toInt());
            });
        addSettingToGrid(layout1, "Ascan Data Size", ascanDataSizeDropdown, 2, 0);

        // Dropdown for Rectification Type
        QComboBox* rectificationDropdown = new QComboBox();
        rectificationDropdown->addItem("None", QVariant::fromValue(Instrumentation::IAmplitudeSettings::RectificationType::None));
        rectificationDropdown->addItem("Positive", QVariant::fromValue(Instrumentation::IAmplitudeSettings::RectificationType::Positive));
        rectificationDropdown->addItem("Negative", QVariant::fromValue(Instrumentation::IAmplitudeSettings::RectificationType::Negative));
        rectificationDropdown->addItem("Full", QVariant::fromValue(Instrumentation::IAmplitudeSettings::RectificationType::Full));
        rectificationDropdown->setCurrentIndex(static_cast<int>(setupConfig->phasing_rectification));

        connect(rectificationDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            setupConfig->phasing_rectification = static_cast<Instrumentation::IAmplitudeSettings::RectificationType>(rectificationDropdown->itemData(index).toInt());
            });
        addSettingToGrid(layout1, "Rectification Type", rectificationDropdown, 2, 1);




        // Group for Gate Settings
        QGroupBox* group2 = new QGroupBox("Gate Settings");
        //group2->setFixedSize(180, 300);
        QVBoxLayout* layout2 = new QVBoxLayout(group2);

        auto gateIDelayInput = createDoubleSpinBox(0.0, 20000.0, setupConfig->gate_gateIDelay, [=](double value) {
            setupConfig->gate_gateIDelay = value;
            });
        addSetting(layout2, "Gate I Delay", gateIDelayInput);

        auto gateILengthInput = createDoubleSpinBox(0.0, 10000.0, setupConfig->gate_gateILength, [=](double value) {
            setupConfig->gate_gateILength = value;
            });
        addSetting(layout2, "Gate I Length", gateILengthInput);

        auto gateIThresholdInput = createDoubleSpinBox(0.0, 100.0, setupConfig->gate_gateIThreshold, [=](double value) {
            setupConfig->gate_gateIThreshold = value;
            });
        addSetting(layout2, "Gate I Threshold", gateIThresholdInput);

        auto gateISamplingResolutionInput = createDoubleSpinBox(1.0, 20.0, setupConfig->gate_gateISamplingResolution, [=](double value) {
            setupConfig->gate_gateISamplingResolution = value;
            });
        addSetting(layout2, "Gate I Sampling Resolution", gateISamplingResolutionInput);


        // Group for Phasing Acquisition Settings
        QGroupBox* group3 = new QGroupBox("Phasing Acquisition Settings");
        //group3->setFixedSize(180, 200);
        QVBoxLayout* layout3 = new QVBoxLayout(group3);
        auto paDigitizingLength = createDoubleSpinBox(0.0, 2000000., setupConfig->PA_DigitizingLength, [=](double value) {
            setupConfig->PA_DigitizingLength = value;
            });
        addSetting(layout3, "Pulser Voltage", paDigitizingLength);

        auto paBeamNumberInput = createDoubleSpinBox(0.0, 2048. , setupConfig->beamNumber, [=](double value) {
            setupConfig->beamNumber = value;
            });
        addSetting(layout3, "Pulser Voltage", paBeamNumberInput);

        auto paVoltageInput = createDoubleSpinBox(0.0, 10.0, setupConfig->acquisition_paVoltage, [=](double value) {
            setupConfig->acquisition_paVoltage = value;
            });
        addSetting(layout3, "Pulser Voltage", paVoltageInput);

        QLineEdit* deviceSerialInput = new QLineEdit(QString::fromStdString(setupConfig->acquisition_deviceSerialNumber));
        connect(deviceSerialInput, &QLineEdit::textChanged, [=](const QString& text) {
            setupConfig->acquisition_deviceSerialNumber = text.toStdString();
            });
        addSetting(layout3, "Device Serial Number", deviceSerialInput);

        group1->setFixedWidth(cwidth*2);
        group2->setFixedWidth(cwidth);
        group3->setFixedWidth(cwidth);

        layout->addWidget(group1);
        layout->addWidget(group2);
        layout->addWidget(group3);

        QFrame* separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        layout->addWidget(separator);
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
