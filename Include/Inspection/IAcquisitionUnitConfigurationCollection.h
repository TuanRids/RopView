///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IAcquisitionUnitConfigurationCollection.h.
///
/// Declares the IAcquisitionUnitConfigurationCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>
#include <Equipment/IAcquisitionUnit.h>
#include <Inspection/IAcquisitionUnitConfiguration.h>

namespace Olympus
{
  namespace Inspection
  {
    class IAcquisitionUnitConfigurationCollection
    {
    public:
      virtual ~IAcquisitionUnitConfigurationCollection() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the device configurations count
      ///
      /// @returns The count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a configuration
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3"
      /// 
      /// @param index Zero-based index of the configuration.
      ///
      /// @returns The configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IAcquisitionUnitConfigurationPtr GetConfiguration(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a configuration
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3"
      /// 
      /// @param index Zero-based index of the configuration.
      ///
      /// @returns The configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IAcquisitionUnitConfigurationConstPtr GetConfiguration(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for the first configuration
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3"
      /// 
      /// @param name The name of the configuration.
      ///
      /// @returns The found configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IAcquisitionUnitConfigurationPtr FindConfiguration(const std::wstring& name) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for the first configuration
      ///
      /// NDE file domain path "acquisitionUnits"[index] / "omniScanX3"
      /// 
      /// Returns a null pointer if no device configuration was found.
      ///
      /// @param name The name of the configuration.
      ///
      /// @returns The found configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IAcquisitionUnitConfigurationConstPtr FindConfiguration(const std::wstring& name) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds device
      ///
      /// @param acqUnit The device to add.
      ///
      /// @returns An IAcquisitionUnitConfigurationPtr.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IAcquisitionUnitConfigurationPtr Add(Equipment::IAcquisitionUnitPtr acqUnit) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the given configuration
      ///
      /// @param name The name of the configuration to remove.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Remove(const std::wstring& name) = 0;
    };

    using IAcquisitionUnitConfigurationCollectionPtr = std::shared_ptr<IAcquisitionUnitConfigurationCollection>;
    using IAcquisitionUnitConfigurationCollectionConstPtr = std::shared_ptr<const IAcquisitionUnitConfigurationCollection>;
  }
}
