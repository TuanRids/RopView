////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IDeviceDiscovery.h
///
/// Declares the device discovery interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <instrumentation/InstrumentationExport.h>
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include "IDeviceInfo.h"

namespace Instrumentation
{
  class IDevice;
  class ILinkedDeviceCollection;

  /*! Wait timeout duration in milliseconds.
   */
  using Duration = long;

  struct DiscoverResult
  {
    enum Status : unsigned
    {
      Spurious = 0,
      Interrupted = 1,
      Timeout = Interrupted << 1,
      Failed = Interrupted << 2,
      DeviceFound = Interrupted << 3,
    };

    Status status;
    std::shared_ptr<IDevice> device;
  };

  class INSTRUMENTATION_API IDeviceDiscovery
  {
  public:
    virtual ~IDeviceDiscovery() = default;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Free the thread blocked in the method "IDeviceDiscovery::Discover"
    ///
    /// The DiscoverResult::Status returned by the method "IDeviceDiscovery::Discover" will be "Interrupted"
    ///
    /// \see IDeviceDiscovery::Discover
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Interrupt() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Try to discover a device
    ///
    /// The method is blocking and will unblock when one of the following condition happens :
    ///
    /// 1 - A device is Found. 
    /// In that case, DiscoverResult::status will be "DeviceFound"
    /// and DiscoverResult::DiscoveredDevice will hold the instance of the newly found device.
    ///
    /// 2 - Manually interrupted. 
    /// The method IDeviceDiscovery::Interrupt will free the blocked thread.
    /// DiscoverResult::Status will be "Interrupted" and DiscoverResult::device will hold a nullptr
    ///
    /// 3 - Timeout is reached. 
    /// DiscoverResult::status will be "Timeout" and DiscoverResult::device will hold a nullptr
    ///
    /// 4 - Network error
    /// The thread couldn't start discovering for various reasons
    /// DiscoverResult::Status will be "Failed" and DiscoverResult::device will hold a nullptr
    ///
    /// 5 - Randomly awaken by the operating system
    /// DiscoverResult::Status will be "Spurious" and DiscoverResult::device will hold a nullptr
    ///
    /// @param duration
    /// Optional parameter specifying the timeout in millisecond on which the thread will unblock
    ///
    /// \return The reason why the thread has been freed, and possibly the discovered device
    ///
    /// \see IDeviceDiscovery::Interrupt
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual DiscoverResult DiscoverFor(Duration duration) = 0;
    virtual DiscoverResult Discover() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create a new device discovery on a network.
    ///
    /// @param ipAddress The IpAddress of the network adapter to use to discover the devices
    ///
    /// @return
    /// The created IDeviceDiscovery interface. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::shared_ptr<IDeviceDiscovery> Create(const std::string& ipAddress);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the linked devices which are connected through a synchro cable or are on the same
    /// active network adapter.
    ///
    /// Devices connectivity configuration must have been applied with <em>Configuration Tool</em> 
    /// executable to get a non-empty collection.
    ///
    /// @return
    /// The linked devices.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::shared_ptr<ILinkedDeviceCollection> GetLinkedDevices();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create a simulated device discovery.
    ///
    /// @param ipAddress The IpAddress of the network adapter to use to discover the devices
    ///
    /// @return
    /// The created IDeviceDiscovery interface. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::shared_ptr<IDeviceDiscovery> CreateSimulatorDiscovery(const std::string& ipAddress, 
                                                                      IDeviceInfo::Platform platform= IDeviceInfo::Platform::FocusPx,
                                                                      IDeviceInfo::Model model= IDeviceInfo::Model::PA32x128);

  };
}
