#ifndef IOmConnect_H
#define IOmConnect_H
#include "..\pch.h"
using namespace Instrumentation;
using namespace std;


class IOmConnect
{
public:
    virtual void omDisconnectDevice() = 0;
    virtual bool omConnectDevice() = 0;
    static std::shared_ptr<IOmConnect> Create(); /*Factory Method*/

protected:
    IOmConnect() : device(nullptr) {}
    virtual ~IOmConnect() = 0;

    shared_ptr<IDevice> device;
    string ipAddress = "192.168.0.1";

    virtual shared_ptr<IDevice> DiscoverDevice() = 0;
    virtual void StartDevice() = 0;
    virtual void ConfigureDevice() = 0;
};
#endif