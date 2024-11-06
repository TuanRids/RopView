#ifndef OPENVIEW_CONFIGURATION_H
#define OPENVIEW_CONFIGURATION_H
#include "../pch.h"
#include "SystemConfig/ConfigLocator.h"
using namespace std;
using namespace Olympus::FileManagement;
using namespace Olympus::Inspection;
using namespace Instrumentation;


namespace OpenView
{
    class Configuration
    {
    public:
        static void Create(ISetupPtr setup);
        static void Read(ISetupPtr setup);

    private:
        static void ConfigureConventional(IConventionalConfigurationPtr config);
        static void ConfigurePhasedArray(IPhasedArrayConfigurationPtr config);

        static void ReadInspectionConfigs(IInspectionConfigurationCollectionConstPtr inspConfigs);
        static void ReadConfigurations(IInspectionConfigurationCollectionConstPtr inspConfigs);
        static void ReadConfiguration(IConfigurationConstPtr config);
        static void ReadThicknessSettings(IThicknessSettingsPtr thicknessSettings);
        static std::shared_ptr<Om_Settup_Config> omSetCof;

    };
}

#endif