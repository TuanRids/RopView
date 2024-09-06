#ifndef READCONFIG_H
#define READCONFIG_H

#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

class readcf {
private:
    static std::string defaultvalue;
    static boost::filesystem::path currentDir;

public:
    static std::string readconfig(const std::string& key, std::string& value = defaultvalue);
};

#endif // READCONFIG_H
