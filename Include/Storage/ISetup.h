////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ISetup.h
///
/// Declares the ISetup Interface.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <instrumentation/IAcquisitionConfigurationCollection.h>
#include <View/IViewProperties.h>

#ifndef SWIG
#include <instrumentation/IAcquisitionConfiguration.h>
#endif

namespace Olympus
{
  namespace Inspection{
    class IInspectionConfigurationCollection;
    using IInspectionConfigurationCollectionPtr = std::shared_ptr<IInspectionConfigurationCollection>;
    using IInspectionConfigurationCollectionConstPtr = std::shared_ptr<const IInspectionConfigurationCollection>;

    class IInspectionConfiguration;
    using IInspectionConfigurationPtr = std::shared_ptr<IInspectionConfiguration>;
    using IInspectionConfigurationConstPtr = std::shared_ptr<const IInspectionConfiguration>;
  }
  namespace PartDefinition {
    class IScanPlan;
    using IScanPlanPtr = std::shared_ptr<IScanPlan>;
    using IScanPlanConstPtr = std::shared_ptr<const IScanPlan>;
  }
  namespace FileManagement {
    class ISignature;
    using ISignaturePtr = std::shared_ptr<ISignature>;
    using ISignatureConstPtr = std::shared_ptr<const ISignature>;

    class ISetup
    {
    public:
      virtual ~ISetup() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a pointer to the acquisition configurations.
      ///
      /// @returns The acquisition configurations.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Inspection::IInspectionConfigurationCollectionConstPtr GetInspectionConfigurations() const = 0;
      virtual Inspection::IInspectionConfigurationCollectionPtr GetInspectionConfigurations() = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a pointer to the scan plan.
      ///
      /// @returns The scan plan.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual PartDefinition::IScanPlanPtr GetScanPlan() = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a constant pointer to the scan plan.
      ///
      /// @returns The scan plan.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual PartDefinition::IScanPlanConstPtr GetScanPlan() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a pointer to the program signature.
      ///
      /// @returns The program signature.
      /////////////////////////////////////////////////////////////////////////////////////////////
      __declspec(deprecated("Deprecated. Use GetFileSignature in IDataFile"))
        virtual ISignatureConstPtr GetSignature() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a constant pointer to the program signature.
      ///
      /// @returns The program signature.
      /////////////////////////////////////////////////////////////////////////////////////////////
      __declspec(deprecated("Deprecated. Use GetFileSignature in IDataFile"))
      virtual ISignaturePtr GetSignature() = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Convert an acquisition configuration coming from an actual acquisition using
      /// Instrumentation::IAcquisitionConfiguration::GetConfiguration() method to the
      /// saved file representation.
      ///
      /// A scan plan representing the original acquisition must be created in the
      /// setup beforehand. The converted acquisition configuration can then be
      /// modified for calibration and adjusting beam or wedge delays. Insertion in
      /// the collection of acquisition configurations for setup is possible with
      /// Inspection::IInspectionConfigurationCollection::Add(Inspection::IInspectionConfigurationPtr)
      /// method.
      /// 
      /// While using this adapter, it is important to understand that the 
      /// instrument representation is a subset of the saved file representation. The user will need to
      /// supply extra information, including but not exclusive to the following : 
      /// 
      /// PhasedArrayConfiguration::Velocity: this uses the scan plan theoretical value, but the calibrated value may be different, 
      /// PhasedArrayConfiguration::WedgeDelay: DigitizingDelay must be reduced from the value added to the wedge delay since AScanStart = BeamDelay + WedgeDelay + DigitizingDelay, 
      /// PhasedArrayConfiguration::ReferenceAmplitude, 
      /// PhasedArrayBeamConfiguration::ExitPoint, 
      /// PhasedArrayBeamConfiguration::BeamDelay, 
      /// PhasedArrayBeamConfiguration::GainOffset, 
      /// GateConfiguration::Delay, 
      /// TCGPoint::Position
      ///
      /// @returns The converted acquisition configuration
      ///
      /// @see IInspectionConfigurationCollection::Add(Inspection::IInspectionConfigurationPtr)
      /////////////////////////////////////////////////////////////////////////////////////////////

      virtual Inspection::IInspectionConfigurationPtr ConvertAcquisitionConfiguration(
        std::shared_ptr<Instrumentation::IAcquisitionConfiguration> acquisitionConfig) = 0;

      virtual Instrumentation::IAcquisitionConfigurationCollectionPtr GetAcquisitionConfigurations() const = 0;

      virtual View::IViewPropertiesConstPtr GetViewProperties() const = 0;

      virtual View::IViewPropertiesPtr GetViewProperties() = 0;
    };

    using ISetupPtr = std::shared_ptr<ISetup>;
    using ISetupConstPtr = std::shared_ptr<const ISetup>;
  }
}
