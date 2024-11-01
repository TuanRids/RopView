#ifndef OPENVIEW_SCANPLAN_H
#define OPENVIEW_SCANPLAN_H
#include "../pch.h"
#include "SystemConfig/ConfigLocator.h"
using namespace std;
using namespace Olympus::FileManagement;
using namespace Olympus::PartDefinition;
using namespace Olympus::Equipment;
using namespace Olympus::Inspection;
using namespace Instrumentation;


namespace OpenView
{
    class ScanPlan
    {
    public:
        static void Create(ISetupPtr setup);
        static void Read(IScanPlanPtr scanPlan);

    private:
        static void AddPulseEchoInspectionMethod(IScanPlanPtr scanPlan);
        static void AddSectorialInspectionMethod(IScanPlanPtr scanPlan);
        static void AddLinearInspectionMethod(IScanPlanPtr scanPlan);
        static void AddTofdInspectionMethod(IScanPlanPtr scanPlan);

        static void ReadSpecimen(IScanPlanPtr scanPlan);
        static void ReadInspectionMethod(IScanPlanPtr scanPlan);
        static void ReadProbeConventional(IProbeConventionalPtr probe);
        static void ReadProbeRectangularArray(IProbePtr probe);
        static std::shared_ptr<Om_Setup_ScanPlan> omSetup;
    };
}

#endif