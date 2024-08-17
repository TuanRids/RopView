#include "loadjson.h"

std::unique_ptr<configsetting> omcn::loadjson::config = std::make_unique<configsetting>();
const string omcn::loadjson::config_path = "settings/config.json";
using json = nlohmann::json;


void omcn::loadjson::loadsetting() {
    if (config->check_loading_config) return; 

    ifstream config_file(config_path);
    if (!config_file) {
        std::cerr << "Error: Could not open config file" << std::endl;
        return;
    }

    try {
        json loadconfig;
        config_file >> loadconfig;

        // Map JSON data to configsetting struct
        config->cnIPaddress = loadconfig.value("ip_address", config->cnIPaddress);
        config->cnPort = loadconfig.value("port", config->cnPort);
        config->cnTimeout = loadconfig.value("timeout", config->cnTimeout);
        config->cnDevicename = loadconfig.value("device_name", config->cnDevicename);

        config->check_loading_config = true;

        // Optionally, print out the loaded configuration to confirm
        cout << "\t IP Address: " << config->cnIPaddress << endl;
        cout << "\t Port: " << config->cnPort << endl;
        cout << "\t Timeout: " << config->cnTimeout << endl;
        cout << "\t Device Name: " << config->cnDevicename << endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Exception during loading config - " << e.what() << std::endl;
    }

}

void omcn::loadjson::savesetting() {
    ofstream config_file(config_path);
    if (!config_file) {
        std::cerr << "Error: Could not open config file for writing: " << config_path << std::endl;
        return;
    }

    try {
        json saveconfig;
        saveconfig["ip_address"] = config->cnIPaddress;
        saveconfig["port"] = config->cnPort;
        saveconfig["timeout"] = config->cnTimeout;
        saveconfig["device_name"] = config->cnDevicename;

        config_file << std::setw(4) << saveconfig << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Exception during saving config - " << e.what() << std::endl;
    }

}

/// <summary>
/// Update the Configuration.
/// </summary>
/// <param name="newconfig"></param>
void omcn::loadjson::updateconfig(const configsetting& newconfig) {
    config->cnIPaddress = newconfig.cnIPaddress;
	config->cnPort = newconfig.cnPort;
	config->cnTimeout = newconfig.cnTimeout;
	config->cnDevicename = newconfig.cnDevicename;
	std::cout << "The config is updated" << std::endl;
    savesetting();
}

// Get configuration
const configsetting& omcn::loadjson::getconfig() {
    if (!config->check_loading_config) {
        loadsetting();
    }
    return *config;
}
