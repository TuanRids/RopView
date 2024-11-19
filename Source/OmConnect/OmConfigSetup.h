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
    OmConfigSetup(IDevicePtr ndevice);

    IAcquisitionPtr ConfigDeviceSetting();
    void ConfigUpdateSetting();
private:
    // getter properties
    IAcquisitionPtr acquisition;
    IDevicePtr device;

    // Internal Properties
    std::shared_ptr<Om_Settup_Config> omSetCof;
    Instrumentation::IUltrasoundConfigurationPtr ultrasoundConfig;
    IConfigurationPtr config;
    IBeamSetPtr beamSet;

    // Internal Methods
    void ConfigureLinearBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureSectorialBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureCompoundBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureDigitizingSettings(IDigitizerTechnologyPtr digitizerTechnology);

};

#endif // CONFIGSETUP_H
