#include "../pch.h"

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
    
};

/// Linear: Linear nonFocus Constant
///         Linear Fix angled (Focus)
/// Sectorial: Center at 1st element
///         
/// Compound Scan.
///
enum class PautModeOmni {Linear = 0, Sectorial = 1, Compound = 2, TFM = 3};
struct VisualizeConfig
{
    //color Palette upper | lowewr
    int Color_Palette = 0;
    uint16_t BC_Scan_Horizontal = 100;
    PautModeOmni setPautMode = PautModeOmni::Linear;
    
};

struct Om_Settup_Config
{
    // ViewScanSetting
    int SviewScaleX = 5;
    std::atomic<bool> SviewExp = false;
    // === Configure a phased array BeamSet
    // Phased Array BeamFormation
    unsigned int beamNumber     = 60;
    unsigned int EleStep        = 1; // If EleStep is too high, take that FocusLength with the high EleStep could increase the pitch virtual, then the beam could cause Lobes: pv > 1/2*(lambda)
    unsigned int EleQuantity    = 16; // Active elements
    unsigned int EleFirst       = 1;    // Starting element
    unsigned int EleLast        = 64;    // Maximum number of elements
    double Ele_Delay = 500; //Element Delay; 500

    double BeamStartAngle = 10;
    // Focus Length only work as near field (about under 40mm). If Focus Length is too high, the Beam symmetry will like a constant Linear Beam, then it will changed to noFocus
    // Focus = 0 is No Focus
    double FocusLength = 0;
    double Velocity = 6750;

    // === Phased Array Digitizer
    Instrumentation::IAmplitudeSettings::AscanDataSize Digi_Ampli_AscanSize = Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits; // 8 default 12 16
    Instrumentation::IAmplitudeSettings::RectificationType Digi_Ampli_rectification = Instrumentation::IAmplitudeSettings::RectificationType::Full; // full negavite none possitive 
    Instrumentation::IAmplitudeSettings::ScalingType Digi_Ampli_Scaling = Instrumentation::IAmplitudeSettings::ScalingType::Linear; // full negavite none possitive 
    int Digi_Time_CompressFactor = 3;
    Instrumentation::ITimeSettings::AscanSynchroMode Digi_Time_Synch = Instrumentation::ITimeSettings::AscanSynchroMode::Absolute;
    Instrumentation::ITimeSettings::SamplingDecimationFactor Digi_Time_DecimaFactor = Instrumentation::ITimeSettings::SamplingDecimationFactor::One;
    int Digi_BandPassFilter = 10;
    
    // === Each Individual Beam
    double BeamGain = 35.0;
    double BeamAStart = 10; // mm 
    double BeamAEnd = 80; // mm
    
    int Rate = 60; // 120 Hz screen

    // === Used for ScanSection
    int beamCurrentID = 5;             // Current ID


};
    