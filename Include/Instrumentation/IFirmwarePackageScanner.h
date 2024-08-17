#pragma once
#include <instrumentation/InstrumentationExport.h>
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IFirmwarePackageCollection;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// The interface for firmware package scanning.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class INSTRUMENTATION_API IFirmwarePackageScanner
  {
  public:
    virtual ~IFirmwarePackageScanner() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Scan folder directories to find the available firmware packages to deploy on the device
    ///
    /// The firmware packages are located in the folder "Firmware Packages"
    /// The method will try to search for the folder next to the executable
    /// If it doesn't find the folder, it will try again in the folder above, up to five time
    /// It will stop
    ///
    /// Ex : 
    /// Searching for "Firmware Packages" folder in "ExecutableDir", not found
    /// Searching for "Firmware Packages" folder in "ExecutableDir\..\", not found
    /// Searching for "Firmware Packages" folder in "ExecutableDir\..\..\", not found
    /// Searching for "Firmware Packages" folder in "ExecutableDir\..\..\..\", found
    /// Stop
    /// 
    /// @return
    /// All the valid packages that were found in the "Firmware Packages" folder
    ///
    /// @see IDevice::Download
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::shared_ptr<IFirmwarePackageCollection> GetFirmwarePackageCollection();
  };
}
