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
    void removeAcquisition();
private:
    // getter properties
    IAcquisitionPtr acquisition;
    IDevicePtr device;

    // Internal Properties
    std::shared_ptr<Om_Settup_Config> omSetCof;
    IBeamSetPtr beamSet;
    shared_ptr<spdlog::logger> debugLogger;
    // Internal Methods
    void ConfigureLinearBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureSectorialBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureCompoundBeam(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureTFM(std::shared_ptr<Instrumentation::IBeamSetFactory> phasedArrayFactory, shared_ptr<IBeamFormationCollection> beamFormations);
    void ConfigureDigitizingSettings(IDigitizerTechnologyPtr digitizerTechnology);
    std::string appendLog(std::string& logger, unsigned int VirAperture, double delayNoFocus , double deltaN, double delay );

};

#endif // CONFIGSETUP_H
