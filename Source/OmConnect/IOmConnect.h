#pragma once
#include "..\pch.h"
#include "MainUI/SettingPara.h"
using namespace Instrumentation;
using namespace std;

struct OmniConfig {
    size_t Rate = 120;                // Hz. Cycle rate that collects data per second
    size_t beamLimit = 64;            // Number of beams
    size_t elementAperture = 16;      // Element aperture size
    double delayResolution = 2.5;     // Delay resolution in nanoseconds
    double pulserBaseDelay = 500;     // Base delay for pulsers
    double receiverBaseDelay = 1000;  // Base delay for receivers
    int ascanStart = 0;               // Ascan start position
    int ascanLength = 20000;          // Ascan length
    int gateStart = 1500;             // Gate start position
    int gateLength = 300;             // Gate length
    int gateThreshold = 15;           // Gate threshold value
};

class IOmConnect
{
protected:
    IOmConnect() : device(nullptr) {}
    virtual ~IOmConnect() = 0; 
    OmniConfig config;
    shared_ptr<IDevice> device;
    string ipAddress = "192.168.0.1";

    virtual shared_ptr<IDevice> DiscoverDevice() = 0;
    virtual void StartDevice() = 0;
    virtual void ConfigureDevice() = 0;

public:
    virtual void omDisconnectDevice() = 0;
    virtual bool omConnectDevice() = 0;
};

inline IOmConnect::~IOmConnect() {}


