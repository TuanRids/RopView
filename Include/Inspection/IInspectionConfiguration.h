///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IInspectionConfiguration.h.
///
/// Declares the IInspectionConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <instrumentation/IAcquisition.h>

namespace Olympus
{
  namespace Inspection
  {
    class IAcquisitionUnitConfigurationCollection;
    using IAcquisitionUnitConfigurationCollectionPtr = std::shared_ptr<IAcquisitionUnitConfigurationCollection>;
    using IAcquisitionUnitConfigurationCollectionConstPtr = std::shared_ptr<const IAcquisitionUnitConfigurationCollection>;

    class IEncoderConfigurationCollection;
    using IEncoderConfigurationCollectionPtr = std::shared_ptr<IEncoderConfigurationCollection>;
    using IEncoderConfigurationCollectionConstPtr = std::shared_ptr<const IEncoderConfigurationCollection>;

    class IConfigurationCollection;
    using IConfigurationCollectionPtr = std::shared_ptr<IConfigurationCollection>;
    using IConfigurationCollectionConstPtr = std::shared_ptr<const IConfigurationCollection>;

    enum class FiringTrigger { Internal, External, Encoder };

    class IInspectionConfiguration
    {
    public:
      virtual ~IInspectionConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the rate of acquisition
      ///
      /// NDE file domain path acquisitionUnits[index] / omniScanX3 / acquisitionPace
      /// 
      /// @returns The rate.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetRate() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a rate of acquisition
      ///
      /// @param rate The rate.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetRate(double rate) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets firing trigger
      ///
      /// NDE file domain path: motionDevices[index] / paceBasedEncoding for Internal trigged else it's Encoder trigged
      /// 
      /// @returns The firing trigger.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual FiringTrigger GetFiringTrigger() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets firing trigger
      ///
      /// @param trigger The trigger.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetFiringTrigger(FiringTrigger trigger) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets inspection configurations
      ///
      /// NDE file domain path dataGroups
      /// 
      /// @returns The configurations.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IConfigurationCollectionPtr GetConfigurations() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets device configurations
      ///
      /// NDE file domain path acquisitionUnits
      /// 
      /// @returns The device configurations.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IAcquisitionUnitConfigurationCollectionPtr GetAcquisitionUnitConfigurations() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets encoder configurations
      ///
      /// NDE file domain path  motionDevices / quadratureEncoder or paceBasedEncoding
      /// 
      /// @returns The encoder configurations.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IEncoderConfigurationCollectionPtr GetEncoderConfigurations() const = 0;
    };

    using IInspectionConfigurationPtr = std::shared_ptr<IInspectionConfiguration>;
    using IInspectionConfigurationConstPtr = std::shared_ptr<const IInspectionConfiguration>;
  }
}

