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
        QGroupBox* group1 = new QGroupBox("Scan Plan");
        QGridLayout* layout1 = new QGridLayout(group1);

        createDoubleSpinBox(1.0, 15.0, scanPlan->raProbeFre, layout1, "Transducer Frequency", 0, 0, [=](double value) {
            scanPlan->raProbeFre = value;
            });

        createSpinBox(1, 128, scanPlan->raProbeElem, layout1, "Element Quantity", 1, 0, [=](int value) {
            scanPlan->raProbeElem = value;
            });

        createSpinBox(1, 10, scanPlan->raProbeRows, layout1, "Row Quantity", 2, 0, [=](int value) {
            scanPlan->raProbeRows = value;
            });

        createDoubleSpinBox(0.1, 5.0, scanPlan->raProbeWidth, layout1, "Width (mm)", 3, 0, [=](double value) {
            scanPlan->raProbeWidth = value;
            });
        createDoubleSpinBox(1.0, 20.0, scanPlan->raProbeHeight, layout1, "Height (mm)", 4, 0, [=](double value) {
            scanPlan->raProbeHeight = value;
            });

        // Group 2 - Phased Array Wedge Settings
        QGroupBox* group2 = new QGroupBox("Wedge Settings");
        QGridLayout* layout2 = new QGridLayout(group2);

        createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeLength, layout2, "Length (mm)", 0, 0, [=](double value) {
            scanPlan->raWedgeLength = value;
            });

        createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeWidth, layout2, "Width (mm)", 1, 0, [=](double value) {
            scanPlan->raWedgeWidth = value;
            });

        createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeHeight, layout2, "Height (mm)", 2, 0, [=](double value) {
            scanPlan->raWedgeHeight = value;
            });

        createDoubleSpinBox(1.0, 100.0, scanPlan->raWedgeExitPoint, layout2, "Exit Point (mm)", 3, 0, [=](double value) {
            scanPlan->raWedgeExitPoint = value;
            });

        createDoubleSpinBox(1000.0, 20000.0, scanPlan->raWedgeMaterialVelocity, layout2, "Material Velocity (m/s)", 4, 0, [=](double value) {
            scanPlan->raWedgeMaterialVelocity = value;
            });

        // Group 3 - Phased Array Linear Formation
        QGroupBox* group3 = new QGroupBox("Linear Formation");
        QGridLayout* layout3 = new QGridLayout(group3);

        createSpinBox(1, 32, scanPlan->EleStep, layout3, "Step", 0, 0, [=](double value) {
            scanPlan->EleStep = value;
            });

        createSpinBox(1, 32, scanPlan->EleQuantity, layout3, "Quantity", 1, 0, [=](int value) {
            scanPlan->EleQuantity = value;
            });

        createSpinBox(1, 32, scanPlan->EleFirst, layout3, "First", 2, 0, [=](int value) {
            scanPlan->EleFirst = value;
            });

        createSpinBox(1, 64, scanPlan->EleLast, layout3, "Last", 3, 0, [=](int value) {
            scanPlan->EleLast = value;
            });

        createSpinBox(1, 64, scanPlan->BandPassFilter, layout3, "BandPassFilter", 4, 0, [=](int value) {
            scanPlan->BandPassFilter = value;
            });

        // Group 4 - Phased Array Beam Settings
        QGroupBox* group4 = new QGroupBox("Beam Settings");
        QGridLayout* layout4 = new QGridLayout(group4);

        createDoubleSpinBox(1.0, 200.0, scanPlan->beamFocusingDepth, layout4, "Focusing Depth (mm)", 0, 0, [=](double value) {
            scanPlan->beamFocusingDepth = value;
            });

        createDoubleSpinBox(0.0, 90.0, scanPlan->beamRefractedAngle, layout4, "Refracted Angle (deg)", 1, 0, [=](double value) {
            scanPlan->beamRefractedAngle = value;
            });

        createDoubleSpinBox(0.0, 360.0, scanPlan->probePositionAngle, layout4, "Probe pos Angle (deg)", 2, 0, [=](double value) {
            scanPlan->probePositionAngle = value;
            });

        createDoubleSpinBox(-100.0, 100.0, scanPlan->probePositionX, layout4, "Probe pos X (mm)", 3, 0, [=](double value) {
            scanPlan->probePositionX = value;
            });

        createDoubleSpinBox(-100.0, 100.0, scanPlan->probePositionY, layout4, "Probe pos Y (mm)", 0, 1, [=](double value) {
            scanPlan->probePositionY = value;
            });

        createDoubleSpinBox(0.0, 90.0, scanPlan->skewStart, layout4, "Skew Start (deg)", 1, 1, [=](double value) {
            scanPlan->skewStart = value;
            });

        createDoubleSpinBox(0.0, 90.0, scanPlan->skewStop, layout4, "Skew Stop (deg)", 2, 1, [=](double value) {
            scanPlan->skewStop = value;
            });

        createDoubleSpinBox(0.1, 10.0, scanPlan->skewStep, layout4, "Skew Step (deg)", 3, 1, [=](double value) {
            scanPlan->skewStep = value;
            });

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


        createDoubleSpinBox(0.0, 200., setupConfig->phasing_gain, layout1, "Gain", 0, 0, [=](double value) {
            setupConfig->phasing_gain = value;
            });

        createDoubleSpinBox(100.0, 20000.0, setupConfig->phasing_velocity, layout1, "Velocity (m/s)", 1, 0, [=](double value) {
            setupConfig->phasing_velocity = value;
            });

        createDoubleSpinBox(0.0, 100.0, setupConfig->phasing_referenceAmplitude, layout1, "Reference Amplitude", 0, 1, [=](double value) {
            setupConfig->phasing_referenceAmplitude = value;
            });

        createSpinBox(10, 500, setupConfig->phasing_pulseWidth, layout1, "Pulse Width (ns)", 1, 1, [=](int value) {
            setupConfig->phasing_pulseWidth = value;
            });


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
        QGridLayout* layout2 = new QGridLayout(group2);

        createDoubleSpinBox(0.0, 20000.0, setupConfig->gate_gateIDelay, layout2, "Gate I Delay", 0, 0, [=](double value) {
            setupConfig->gate_gateIDelay = value;
            });

        createDoubleSpinBox(0.0, 10000.0, setupConfig->gate_gateILength, layout2, "Gate I Length", 1, 0, [=](double value) {
            setupConfig->gate_gateILength = value;
            });

        createDoubleSpinBox(0.0, 100.0, setupConfig->gate_gateIThreshold, layout2, "Gate I Threshold", 2, 0, [=](double value) {
            setupConfig->gate_gateIThreshold = value;
            });

        createDoubleSpinBox(1.0, 20.0, setupConfig->gate_gateISamplingResolution, layout2, "Gate I Sampling Res", 3, 0, [=](double value) {
            setupConfig->gate_gateISamplingResolution = value;
            });


        // Group for Phasing Acquisition Settings
        QGroupBox* group3 = new QGroupBox("Phasing Acquisition Settings");
        //group3->setFixedSize(180, 200);
        QGridLayout* layout3 = new QGridLayout(group3);
        createDoubleSpinBox(0.0, 2000000., setupConfig->PA_DigitizingLength, layout3, "Digitizer Length", 0, 0, [=](double value) {
            setupConfig->PA_DigitizingLength = value;
            });

        createDoubleSpinBox(0.0, 2048. , setupConfig->beamNumber, layout3, "Beam Number", 1, 0, [=](double value) {
            setupConfig->beamNumber = value;
            });

        createDoubleSpinBox(0.0, 10.0, setupConfig->acquisition_paVoltage, layout3, "Pulser Voltage", 2, 0, [=](double value) {
            setupConfig->acquisition_paVoltage = value;
            });
        createDoubleSpinBox(-360, 360.0, setupConfig->PA_ElemInternalDelay, layout3, "Element Internal Delay", 3, 0, [=](double value) {
            setupConfig->PA_ElemInternalDelay = value;
            });

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
