///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IAcquisitionUnit.h.
///
/// NDE file domain path "acquistionUnits" / "omniScanX3"
///
/// Declares the IAcquisitionUnit interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <instrumentation/IDeviceInfo.h>
#include <instrumentation/DevicePorts.h>

namespace Olympus { namespace Equipment {

  class IInputOutputPort;
  using IInputOutputPortPtr = std::shared_ptr<IInputOutputPort>;
  using IInputOutputPortConstPtr = std::shared_ptr<const IInputOutputPort>;

  class IMultiPulserReceiverPort;
  using IMultiPulserReceiverPortPtr = std::shared_ptr<IMultiPulserReceiverPort>;
  using IMultiPulserReceiverPortConstPtr = std::shared_ptr<const IMultiPulserReceiverPort>;

  class ISinglePulserReceiverPort;
  using ISinglePulserReceiverPortPtr = std::shared_ptr<ISinglePulserReceiverPort>;
  using ISinglePulserReceiverPortConstPtr = std::shared_ptr<const ISinglePulserReceiverPort>;

class IAcquisitionUnit
{
public:
  virtual ~IAcquisitionUnit() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the name of the device
      ///
      /// NDE file domain path "acquistionUnits" / "omniScanX3" / "name"
      /// 
      /// @returns The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the platform of the device
      ///
      /// NDE file domain path "acquistionUnits" / "omniScanX3"
      /// 
      /// @returns The platform.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IDeviceInfo::Platform GetPlatform() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the model of the device
      ///
      /// NDE file domain path "acquistionUnits" / "omniScanX3" / "model"
      /// 
      /// @returns The model.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Instrumentation::IDeviceInfo::Model GetModel() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets single pulser receiver port count
      ///
      /// @returns The single pulser receiver port count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetSinglePulserReceiverPortCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets multi pulser receiver port count
      ///
      /// @returns The multi pulser receiver port count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetMultiPulserReceiverPortCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets single pulser receiver port
      ///
      /// @param index Zero-based index of the port.
      ///
      /// @returns The single pulser receiver port.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ISinglePulserReceiverPortPtr GetSinglePulserReceiverPort(std::size_t index) = 0;
      virtual ISinglePulserReceiverPortConstPtr
      GetSinglePulserReceiverPort(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets multi pulser receiver port
      ///
      /// @param index Zero-based index of the port.
      ///
      /// @returns The multi pulser receiver port.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IMultiPulserReceiverPortPtr GetMultiPulserReceiverPort(std::size_t index) = 0;
      virtual IMultiPulserReceiverPortConstPtr
      GetMultiPulserReceiverPort(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for a single pulser receiver port
      ///
      /// @param name The name.
      ///
      /// @returns The found single pulser receiver port.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ISinglePulserReceiverPortPtr FindSinglePulserReceiverPort(const std::wstring& name) = 0;
      virtual ISinglePulserReceiverPortConstPtr
      FindSinglePulserReceiverPort(const std::wstring& name) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for a multi pulser receiver port
      ///
      /// @param name The name of the port.
      ///
      /// @returns The found multi pulser receiver port.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IMultiPulserReceiverPortPtr FindMultiPulserReceiverPort(const std::wstring& name) = 0;
      virtual IMultiPulserReceiverPortConstPtr
      FindMultiPulserReceiverPort(const std::wstring& name) const = 0;
      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets input output port count
      ///
      /// @returns The input output port count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetInputOutputPortCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets input output port
      ///
      /// @param index Zero-based index of the port.
      ///
      /// @returns The input output port.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInputOutputPortConstPtr
      GetInputOutputPort(std::size_t index) const = 0;
      virtual IInputOutputPortPtr
      GetInputOutputPort(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Finds this device's InputOutputPort by name
      ///
      /// Returns a null pointer if the InputOutputPort is not found.
      ///
      /// @returns The found input output port.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInputOutputPortPtr FindInputOutputPort(const std::wstring& name) = 0;
      virtual IInputOutputPortConstPtr
      FindInputOutputPort(const std::wstring& name) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Connects a firing synchro input
      ///
      /// The input device synched with this device cannot be itself
      /// or one that is synched with the device (form a circular synching link).
      ///
      /// @param device The device.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetFiringSynchroInput(std::shared_ptr<IAcquisitionUnit> device) = 0;

      virtual std::shared_ptr<IAcquisitionUnit> GetFiringSynchroInput() const = 0;
    };

    using IAcquisitionUnitPtr =  std::shared_ptr<IAcquisitionUnit>;
    using IAcquisitionUnitConstPtr = std::shared_ptr<const IAcquisitionUnit>;
  }
}
