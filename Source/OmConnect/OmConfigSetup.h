#ifndef CONFIGSETUP_H
#define CONFIGSETUP_H
#include "SystemConfig/ConfigLocator.h"
#include "../pch.h"

using namespace std::experimental::filesystem;
using namespace Instrumentation;
using namespace Olympus::Equipment;
using namespace Olympus::Inspection;
using namespace Olympus::FileManagement;
using namespace Olympus::FileManagement::Storage;

using namespace std;
class OmConfigSetup {
public:
	static bool ConfigAcquisitionFromSetup(IAcquisitionPtr acquisition, Olympus::FileManagement::ISetupPtr setup);
	static bool ConfigDeviceFromSetup(IAcquisitionPtr nacquisition, IDevicePtr ndevice, Olympus::FileManagement::ISetupPtr setup);
	static bool ConfigDeviceSetting(IAcquisitionPtr nacquisition, IDevicePtr ndevice, Olympus::FileManagement::ISetupPtr setup);
private:
	static IBeamSetPtr Set_CreateFiringBeamSetPhasedArray(IConfigurationPtr config);
	static bool Set_ConfigBeamSetPhasedArray(shared_ptr<IBeamSet> beamSet, IConfigurationPtr config);
	static Instrumentation::IUltrasoundConfigurationPtr ultrasoundConfig ;

	static bool CreateFiringBeamSetPhasedArray();
	static bool UltrasoundConfiguration();
	static std::shared_ptr<Om_Settup_Config> omSetCof;
	static Olympus::FileManagement::ISetupPtr SetupConfig;
};



#endif