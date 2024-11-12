#ifndef IOmConnect_H
#define IOmConnect_H
#include "..\pch.h"


using namespace Instrumentation;
using namespace std;
enum class ConnectMode {
    TestingMode,
    SetupFileMode   
};

class IOmConnect
{
public:
    virtual bool omConnectDevice(ConnectMode mode) = 0;
    static std::shared_ptr<IOmConnect> Create(); /*Factory Method*/
    static void ReleaseDevice();
protected:
    IOmConnect() : device(nullptr) {}
    virtual void omDisconnectDevice() = 0;
    virtual ~IOmConnect() = 0;

    IDevicePtr device;
    string ipAddress = "192.168.0.1";

    virtual void DiscoverDevice(IDevicePtr& device) = 0;
private:

};

#endif