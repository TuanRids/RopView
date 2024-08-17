#pragma once

#include "..\pch.h"
#include "loadjson.h"
using namespace std;
using namespace Instrumentation;

namespace omcn
{
    class OmConnect
    {
    private:
        // properties
        shared_ptr<IDevice> device;
        string ipAddress = "172.17.0.1";

        // private methods
        shared_ptr<IDevice> DiscoverDevice();
        void StartDevice();
        void ConfigureDevice();
    public:
        OmConnect(): device(nullptr) {};
        ~OmConnect() {};
        bool omConnectDevice();
    };

}
