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
    OmConfigSetup(IAcquisitionPtr nacquisition, IDevicePtr ndevice, Olympus::FileManagement::ISetupPtr nsetup);

    bool ConfigDeviceFromSetup();
    bool ConfigDeviceSetting();

    // recheck carefully about shared_ptr and weak_ptr to make sure there are no memory leak
    void get_beamSet(std::shared_ptr<IBeamSet> &beamSet);
    void get_acquisition(IAcquisitionPtr &nacquisition);

private:
    // getter properties
    IAcquisitionPtr acquisition;
    IDevicePtr device;
    Olympus::FileManagement::ISetupPtr setup;

    // Internal Properties
    std::shared_ptr<Om_Settup_Config> omSetCof;
    Instrumentation::IUltrasoundConfigurationPtr ultrasoundConfig;
    IConfigurationPtr config;

    // External Sharing Properties
    std::shared_ptr<IBeamSet> beamSet;

    // Internal Methods for processing.
    IBeamSetPtr Set_CreateFiringBeamSetPhasedArray();
    bool Set_ConfigBeamSetPhasedArray();
    bool CreateFiringBeamSetPhasedArray();
    bool UltrasoundConfiguration();
};

#endif // CONFIGSETUP_H
