#pragma once
#include "..\pch.h"
#include "MainUI/SettingPara.h"
using namespace Instrumentation;
using namespace std;


class IOmConnect
{
protected:
    // Abstract class should have protected constructor/destructor
    IOmConnect() : device(nullptr) {}
    virtual ~IOmConnect() = 0; // Pure virtual destructor makes the class abstract

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


