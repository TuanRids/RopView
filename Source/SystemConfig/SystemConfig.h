#include "../pch.h"
struct OmniConfig {
    /*Testing Realtime Setting*/
    size_t BeamPosition = 0;          // Beam position
    size_t Rate = 120;                // Hz. Cycle rate that collects data per second
    size_t beamLimit = 32;            // Number of beams
    size_t elementAperture = 32;      // Element aperture size
    double delayResolution = 2.5;     // Delay resolution in nanoseconds
    double pulserBaseDelay = 500;     // Base delay for pulsers
    double receiverBaseDelay = 1000;  // Base delay for receivers
    int ascanStart = 5000;            // Ascan start position
    int ascanLength = 20000;          // Ascan length
    int gateStart = 1500;             // Gate start position
    int gateLength = 300;             // Gate length
    int gateThreshold = 15;           // Gate threshold value
};

struct SettingConfig {
    bool bhighResBscan = true;
    std::string ipAddress = "192.168.0.1";
    unsigned int port = 8080;
    unsigned int timeout = 5000;
    std::string device_name = "RoqView";
    std::string qsTheme = "Takezo.qss";
};

struct SystemParams {
    // Resolution scale - offline
    unsigned int resolution = 12;
    // cscan layer
    bool isCscanLayer = false;
    std::string tempBufferLogs ="";
    //color Palette upper | lowewr
    int colorPalette = 0;
    
};

struct Om_Settup_Config
{
    // === Om Phased Array Beam Settings ===
    static constexpr const size_t PA_BeamQty = 32;
    static constexpr const size_t PA_ElementQty = 32;
    static constexpr const size_t PA_FirstElement = 1;
    static constexpr const double PA_BeamAngleStart = 45.;
    double PA_exitPoint[PA_BeamQty] = { 28., 27.75, 27.5, 27.25, 27. }; // mm
    double PA_beamDelay[PA_BeamQty] = { 19000., 19100., 19200., 19300., 19400. }; // ns
    double PA_digitizingDelay[PA_BeamQty] = { 5400., 5450., 5500., 5550., 5600. }; // ns
    double PA_elementDelay[PA_ElementQty] = { 374., 355., 336., 315., 294., 272., 249., 225., 200., 174., 148., 120., 91., 62., 31., 0. };
    double PA_skewAngle = 33.0;

    // === Om Phasing Settings ===
    double phasing_gain = 25.0;
    double phasing_velocity = 3235.0;
    double phasing_referenceAmplitude = 80.0;
    size_t phasing_pulseWidth = 100; // ns
    Instrumentation::IAmplitudeSettings::AscanDataSize phasing_ascanDataSize = Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits; // 8 default 12 16
    Instrumentation::IAmplitudeSettings::RectificationType phasing_rectification = Instrumentation::IAmplitudeSettings::RectificationType::Full; // full negavite none possitive 

    // === Om Gate Settings ===
    double gate_gateIDelay = 5500.0;
    double gate_gateILength = 1500.0;
    double gate_gateIThreshold = 60.0;
    double gate_gateISamplingResolution = 10.0;

    double gate_gateADelay = 6000.0;
    double gate_gateALength = 2000.0;
    double gate_gateAThreshold = 35.0;
    bool gate_gateAReserveBuffer = true;
    double gate_gateASamplingResolution = 10.0;    
    Olympus::Inspection::GatePlacement gate_gateAPlacement = Olympus::Inspection::GatePlacement::SoundPath;

    double gate_gateBDelay = 9000.0;
    double gate_gateBLength = 2500.0;
    double gate_gateBThreshold = 45.0;
    bool gate_gateBReserveBuffer = true;
    double gate_gateBSamplingResolution = 10.0;

    // === Om Phasing Acquisition Settings ===
    double acquisition_paVoltage = 4.0; // Pulser voltage
    std::string acquisition_deviceSerialNumber = "QC-0036707";
};

struct Om_Setup_ScanPlan
{
    // Phased Array Transducer Settings
    double raTransducerFrequency = 5.0;  // MHz
    size_t raTransducerElementQty = 32;  // Number of elements
    size_t raTransducerRowQty = 1;       // Number of rows
    double raTransducerWidth = 0.6;      // Element width (mm)
    double raTransducerHeight = 10.0;    // Element height (mm)

    // Phased Array Wedge Settings
    double raWedgeLength = 48.58;       // Wedge length (mm)
    double raWedgeWidth = 30.0;         // Wedge width (mm)
    double raWedgeHeight = 32.26;       // Wedge height (mm)
    double raWedgeExitPoint = 36.1;     // Exit point (mm)
    double raWedgeMaterialVelocity = 2330.0;  // Material velocity (m/s)

    // Phased Array Linear Formation
    double linearFormationElementStep = 1.0; // Element step size
    size_t linearFormationActiveElements = 32; // Active elements
    size_t linearFormationStartElement = 1;    // Starting element
    size_t linearFormationMaxElements = 64;    // Maximum number of elements

    // Phased Array Beam Settings
    double beamFocusingDepth = 50.0;   // Focusing distance (mm)
    double beamRefractedAngle = 45.0;  // Starting refracted angle (degrees)
    double probePositionAngle = 90.0;  // Probe position angle (degrees)
    double probePositionX = 0.0;       // X position of probe
    double probePositionY = -20.0;     // Y position of probe
    double skewStart = 45.0;           // Skew angle start (degrees)
    double skewStop = 45.0;            // Skew angle stop (degrees)
    double skewStep = 1.0;             // Skew angle step (degrees)

    // Phased Array Acquisition Port
    std::string paPort = "PA_PORT";   // Acquisition unit port for phased array
};
    