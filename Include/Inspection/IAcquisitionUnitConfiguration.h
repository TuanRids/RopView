///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IAcquisitionUnitConfiguration.h.
///
/// Declares the IAcquisitionUnitConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>
#include <instrumentation/IDeviceInfo.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Olympus
{
  namespace Inspection
  {
    class IDigitalInputPinConfigurationCollection;
    using IDigitalInputPinConfigurationCollectionPtr = std::shared_ptr<IDigitalInputPinConfigurationCollection>;
    using IDigitalInputPinConfigurationCollectionConstPtr = std::shared_ptr<const IDigitalInputPinConfigurationCollection>;

    class IUltrasoundDigitizerConfiguration;
    using IUltrasoundDigitizerConfigurationPtr = std::shared_ptr<IUltrasoundDigitizerConfiguration>;
    using IUltrasoundDigitizerConfigurationConstPtr = std::shared_ptr<const IUltrasoundDigitizerConfiguration>;

    class IAcquisitionUnitConfiguration
    {
    public:
      virtual ~IAcquisitionUnitConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the name of the device
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3" / "name"
      /// 
      /// @returns The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the platform of the device
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3"
      /// 
      /// @returns The platform.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IDeviceInfo::Platform GetPlatform() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the model of the device
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3" / "model"
      /// 
      /// @returns The model.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IDeviceInfo::Model GetModel() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets ultrasound digitizer configuration
      ///
      /// @returns The ultrasound digitizer configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IUltrasoundDigitizerConfigurationPtr
        GetUltrasoundDigitizerConfiguration() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the serial number of the device
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3" / "serialNumber"
      /// 
      /// @returns The serial number.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::string& GetSerialNumber() const = 0;


      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the serial number of the device
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3" / "serialNumber"
      /// 
      /// @param serialNumber The serial number.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetSerialNumber(const std::string& serialNumber) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the digital input pin configuration collection
      ///
      /// @returns The digital input pin configuration collection.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IDigitalInputPinConfigurationCollectionPtr
        GetDigitalInputPinConfigurationCollection() const = 0;
    };

    using IAcquisitionUnitConfigurationPtr = std::shared_ptr<IAcquisitionUnitConfiguration>;
    using IAcquisitionUnitConfigurationConstPtr = std::shared_ptr<const IAcquisitionUnitConfiguration>;
  }
}
