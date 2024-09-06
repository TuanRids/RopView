#include "readconfig.h"


std::string readcf::defaultvalue = "";
boost::filesystem::path readcf::currentDir = boost::filesystem::current_path();

std::string readcf::readconfig(const std::string& key, std::string& value) {
    // Return default if file not found
    std::ifstream configFile(currentDir.string() + "/config.ini");
    if (!configFile.is_open()) {
        return defaultvalue;
    }

    // Read the file
    std::string line;
    while (std::getline(configFile, line))
    {
        // Ignore comments
        if (line.empty() || line[0] == '[' || line[0] == ';') {
            continue;
        }

        // Parse the line
        std::istringstream iss(line);
        std::string k, temp, getvalue;
        if (iss >> k >> temp >> getvalue) {
            if (k == key && temp == "=") {
                value = getvalue;
                return getvalue;
            }
        }
    }

    return defaultvalue;
}
