#ifndef OM_CREATE_SETUP
#define OM_CREATE_SETUP

using namespace std;
using namespace std::experimental::filesystem;
using namespace Instrumentation;
using namespace Olympus::Equipment;
using namespace Olympus::Inspection;
using namespace Olympus::PartDefinition;
using namespace Olympus::FileManagement;


class OmCreateSetup
{
public:
	static bool SaveSetup();
private:
	static void ConfigureParameters(ISetupPtr setup);
	static void ConfigureConventional(IConventionalConfigurationPtr config);
	static void ConfigurePhasedArray(IPhasedArrayConfigurationPtr config);
};



#endif