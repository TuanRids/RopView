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
    double PA_exitPoint = 28; // mm
    double PA_beamDelay = 19000; // ns
    double PA_digitizingDelay = 5400; // 5400 ns
    double PA_elementDelay = 500; //Element Delay; 500
    double PA_skewAngle = 33.0;
    double PA_DigitizingLength = 20840.; // Digitizing Length; 163840 ns
            // === Om Phasing Acquisition Settings ===
    double acquisition_paVoltage = 4.0; // Pulser voltage
    std::string acquisition_deviceSerialNumber = "QC-0036707";
    unsigned int beamNumber = 256;

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


};

struct Om_Setup_ScanPlan
{
    // Phased Array Transducer Settings
    double raProbeFre = 5.0;  // MHz
    unsigned int raProbeElem = 64;  // Number of elements
    unsigned int raProbeRows = 1;       // Number of rows
    double raProbeWidth = 0.6;      // Element width (mm)
    double raProbeHeight = 10.0;    // Element height (mm)

    // Phased Array Wedge Settings 
    double raWedgeLength = 48.58;       // Wedge length (mm)
    double raWedgeWidth = 30.0;         // Wedge width (mm)
    double raWedgeHeight = 32.26;       // Wedge height (mm)
    double raWedgeExitPoint = 36.1;     // Exit point (mm)
    double raWedgeMaterialVelocity = 2330.0;  // Material velocity (m/s)

    // Phased Array Linear Formation
    double LinearElemStep = 1.0; // Element step size
    unsigned int LinearElemActive = 32; // Active elements
    unsigned int LinearElemStart = 1;    // Starting element
    unsigned int LinearElemMax = 64;    // Maximum number of elements
    Olympus::Inspection::WaveType LineaerWaveType = Olympus::Inspection::WaveType::Longitudinal; // wavetype

    // Phased Array Beam Settings
    double beamFocusingDepth = 50.0;   // Focusing distance (mm)
    double beamRefractedAngle = 45.0;  // Starting refracted angle (degrees)
    double probePositionAngle = 0;  // Probe position angle (degrees)
    double probePositionX = 0.0;       // X position of probe
    double probePositionY = 0.0;     // Y position of probe
    double skewStart = 45.0;           // Skew angle start (degrees)
    double skewStop = 45.0;            // Skew angle stop (degrees)
    double skewStep = 1.0;             // Skew angle step (degrees)

    // Phased Array Acquisition Port
    std::string paPort = "PA_PORT";   // Acquisition unit port for phased array
};
    