#pragma once
#include "..\pch.h"
#include "MainUI/SettingPara.h"
using namespace Instrumentation;
using namespace std;

struct OmniConfig {
    size_t Rate = 120; // Hz. Cycle rate that collect data per seconds. If too high => setrate will return false and get default
};

class IOmConnect
{
protected:
    // Abstract class should have protected constructor/destructor
    IOmConnect() : device(nullptr) {}
    virtual ~IOmConnect() = 0; // Pure virtual destructor makes the class abstract
    OmniConfig config;
    // properties
    shared_ptr<IDevice> device;
    string ipAddress = "192.168.0.1";

    // private methods
    virtual shared_ptr<IDevice> DiscoverDevice() = 0;
    virtual void StartDevice() = 0;
    virtual void ConfigureDevice() = 0;

public:
    // Public methods
    virtual bool omConnectDevice() = 0;
};

// Definition of the pure virtual destructor
inline IOmConnect::~IOmConnect() {}


