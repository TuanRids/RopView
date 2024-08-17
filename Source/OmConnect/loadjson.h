// loadjson.h
#pragma once
#include "..\pch.h"

using namespace std;

//declare
namespace omcn {
    class loadjson;  // Forward declaration to avoid circular dependency
}


/// <summary>
/// Configuration settings
/// </summary>
struct configsetting {
private:
    string cnIPaddress = "192.168.0.1";
    unsigned int cnPort = 8000;
    unsigned int cnTimeout = 5000;
    string cnDevicename = "OmDevice";


    bool check_loading_config = false; // only used inside to load 1 time
    // Granting access to loadjson to modify the settings
    friend class omcn::loadjson;

public:
    const string& getIPaddress() const { return cnIPaddress; }
    const unsigned int& getPort() const { return cnPort; }
    const unsigned int& getTimeout() const { return cnTimeout; }
    const string& getDevicename() const { return cnDevicename; }
};

/// <summary>
/// Load json settings
/// </summary>
namespace omcn {
    class loadjson {
    private:
        static const string config_path;
        static std::unique_ptr<configsetting> config;
        static void loadsetting();
        void savesetting();
    public:
        void updateconfig(const configsetting& newconfig);
		// Get the configuration
        static const configsetting& getconfig();
    };
}