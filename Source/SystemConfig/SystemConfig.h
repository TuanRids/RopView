#include "../pch.h"
struct OmniConfig {
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
    int colorPalette = 2;
    
};

