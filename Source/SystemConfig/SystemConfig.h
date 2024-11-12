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
    //color Palette upper | lowewr
    int colorPalette = 0;
    
};

struct Om_Settup_Config
{
    // === Configure a phased array BeamSet
    // Phased Array BeamFormation
    unsigned int beamNumber     = 64;
    unsigned int EleStep        = 1; // Element step size
    unsigned int EleQuantity    = 16; // Active elements
    unsigned int EleFirst       = 1;    // Starting element
    unsigned int EleLast        = 64;    // Maximum number of elements
    double Ele_Delay = 500; //Element Delay; 500

    // === Phased Array Digitizer
    Instrumentation::IAmplitudeSettings::AscanDataSize Digi_Ampli_AscanSize = Instrumentation::IAmplitudeSettings::AscanDataSize::TwelveBits; // 8 default 12 16
    Instrumentation::IAmplitudeSettings::RectificationType Digi_Ampli_rectification = Instrumentation::IAmplitudeSettings::RectificationType::Full; // full negavite none possitive 
    Instrumentation::IAmplitudeSettings::ScalingType Digi_Ampli_Scaling = Instrumentation::IAmplitudeSettings::ScalingType::Linear; // full negavite none possitive 
    int Digi_Time_CompressFactor = 10;
    Instrumentation::ITimeSettings::AscanSynchroMode Digi_Time_Synch = Instrumentation::ITimeSettings::AscanSynchroMode::Absolute;
    Instrumentation::ITimeSettings::SamplingDecimationFactor Digi_Time_DecimaFactor = Instrumentation::ITimeSettings::SamplingDecimationFactor::One;
    int Digi_BandPassFilter = 10;
    
    // === Each Individual Beam
    double BeamGain = 35.0;
    double BeamAStart = 4000; // 4000
    double BeamAEnd = 35000; // 35000 ns
    
    int Rate = 245; // 120 Hz screen

    // === Used for ScanSection
    int beamCurrentID = 0;             // Current ID


};
    