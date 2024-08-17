///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment/IPulserReceiverPort.h.
///
/// NDE file domain path: Not available
/// 
/// Declares the IPulserReceiverPort interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>

namespace Olympus { namespace Equipment
{
  class IAcquisitionUnit;
  using IAcquisitionUnitPtr = std::shared_ptr<IAcquisitionUnit>;
  using IAcquisitionUnitConstPtr = std::shared_ptr<const IAcquisitionUnit>;

class IPulserReceiverPort
{
public:
  virtual ~IPulserReceiverPort() = default;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets the name
  ///
  /// @returns The name.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual const std::wstring& GetName() const = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets the device
  ///
  /// @returns The device.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual IAcquisitionUnitPtr GetAcquisitionUnit() const = 0;
};

using IPulserReceiverPortPtr = std::shared_ptr<IPulserReceiverPort>;
using IPulserReceiverPortConstPtr = std::shared_ptr<const IPulserReceiverPort>;
}}
