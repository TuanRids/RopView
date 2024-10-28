#ifndef CONFIGSETUP_H
#define CONFIGSETUP_H
#include "SystemConfig/ConfigLocator.h"
#include "../pch.h"
#include "IOmConnect.h"
using namespace std;
class OmConfigSetup {
public:

	static Olympus::FileManagement::ISetupPtr initSetup();
	static bool ConfigAcquisitionFromSetup(IAcquisitionPtr acquisition, Olympus::FileManagement::ISetupPtr setup);
	static bool ConfigDeviceFromSetup(IDevicePtr device, Olympus::FileManagement::ISetupPtr setup);
	
private:

};



#endif