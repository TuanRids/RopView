////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file instrumentation/ILinkedDeviceCollection.h
///
/// Collection of devices linked through a synchro cable or connected on the
/// same active network adapter.
///
/// The information on the synchro cable links and the active network adapter
/// comes from the device connectivity configuration through ConfigurationTool
/// executable. Devices that are linked with a synchro cable defines a group
/// which are indexed from 0. In a group, devices are also indexed from 0
/// and the first device is the first parent device, i.e.: the device
/// which have a synch. out connected but not its synch. in connector. A group
/// can then create a synced device collection with the first parent as the
/// master.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <string>

namespace Instrumentation
{
  class IDevice;
  class ISyncedDeviceCollection;

  class ILinkedDeviceCollection
  {
  public:
    virtual ~ILinkedDeviceCollection() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the number of groups.
    ///
    /// @return
    /// The number of groups.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetGroupCount() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the total devices count in all groups.
    ///
    /// @return
    /// The total device count in all groups.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetDevicesCount() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the devices count for a group.
    ///
    /// @param groupIndex Index of the group.
    /// @throw std::out_of_range Group index is invalid.
    /// @return
    /// The device count for a group.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetDevicesCount(std::size_t groupIndex) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Find a device by its name.
    ///
    /// @param deviceName Index of the group.
    /// @return
    /// The device found. Null if device was not found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDevice> FindDevice(const std::wstring& deviceName) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get a device by its group and device index.
    ///
    /// @param groupIndex Index of the group.
    /// @param deviceIndex Index of the device.
    /// @throw std::out_of_range Group or device index is invalid.
    /// @return
    /// The device.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDevice> GetDevice(std::size_t groupIndex, std::size_t deviceIndex) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the parent device of a device identified by its name.
    ///
    /// @param name Name of the device from which the parent is determined.
    /// @return
    /// The parent device. Null if device with name \p name is not found or if the device has no parent
    /// (is the first parent).
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDevice> GetParentDevice(std::shared_ptr<IDevice> device) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the first parent device of a device.
    ///
    /// @param device Device from which the parent is determined.
    /// @return
    /// The first parent device.  Null if device \p device is not found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDevice> GetFirstParentDevice(std::shared_ptr<IDevice> device) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the first parent device in the group.
    ///
    /// @param name Index of the group.
    /// @throw std::out_of_range Group index is invalid.
    /// @return
    /// The first parent device.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IDevice> GetFirstParentDevice(std::size_t groupIndex) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create a synced device collection from a group.
    ///
    /// @param name Index of the group
    /// @throw std::out_of_range Group index is invalid.
    /// @return
    /// The synced device collection
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<ISyncedDeviceCollection> CreateSyncedDeviceCollection(std::size_t groupIndex) const = 0;
  };

  using ILinkedDeviceCollectionPtr = std::shared_ptr<ILinkedDeviceCollection>;
  using ILinkedDeviceCollectionConstPtr = std::shared_ptr<const ILinkedDeviceCollection>;
}
