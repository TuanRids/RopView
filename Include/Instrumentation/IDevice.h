////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IDevice.h
///
/// Declares the device interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IDeviceConfiguration;
  class IFirmwarePackage;
  class IDeviceInfo;
  class IDevice
  {
  public:

    enum class State { Idle, Ready, Unreachable};
    virtual ~IDevice() = default;
    
    virtual void Start(std::shared_ptr<IFirmwarePackage> firmwarePackage) = 0;
    virtual void Stop() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Download the firmware package from the PC to the device.
    ///
    /// @prerequisites
    /// - Having a functional network adapter with the IP address "192.168.0.1" enabled on the PC
    /// - Having the TCP port 21 not used by another application
    /// - The firewall must be disabled for the network adapter or a traffic rule must exist for the process
    /// 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Download(std::shared_ptr<IFirmwarePackage> firmwarePackage) = 0;
    virtual bool HasPackage(std::shared_ptr<IFirmwarePackage> firmwarePackage) const = 0;
    virtual std::shared_ptr<IFirmwarePackage> GetStartedPackage() const = 0;
    virtual State GetState() const = 0;
    virtual std::shared_ptr<IDeviceInfo> GetInfo() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the device configuration.
    ///
    /// @return
    /// The device configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDeviceConfiguration> GetConfiguration() const = 0;


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the driver configuration.
    ///
    /// @return
    /// Get the driver configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::string GetDriverConfiguration() const = 0;

    virtual bool ResetConfiguration() = 0;
  };

  using IDevicePtr = std::shared_ptr<IDevice>;
  using IDeviceConstPtr = std::shared_ptr<const IDevice>;
}
