///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment/ISingleElementConnectorSplitter.h.
///
/// NDE file domain path: Not available
/// 
/// Declares the ISingleElementConnectorSplitter interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/ISinglePulserReceiverPort.h>

namespace Olympus { namespace Equipment {

class ISingleElementConnectorSplitter : public ISinglePulserReceiverPort
{
public:
  virtual ~ISingleElementConnectorSplitter() = default;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets pulser connector
  ///
  /// @returns The pulser connector.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISingleElementConnectorPtr GetPulserConnector() = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets pulser connector
  ///
  /// @returns The constant pulser connector.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISingleElementConnectorConstPtr GetPulserConnector() const = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets receiver connector
  ///
  /// @returns The receiver connector.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISingleElementConnectorPtr GetReceiverConnector() = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets receiver connector
  ///
  /// @returns The constant receiver connector.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISingleElementConnectorConstPtr GetReceiverConnector() const = 0;
};

using ISingleElementConnectorSplitterPtr = std::shared_ptr<ISingleElementConnectorSplitter>;
using ISingleElementConnectorSplitterConstPtr = std::shared_ptr<const ISingleElementConnectorSplitter>;
}}
