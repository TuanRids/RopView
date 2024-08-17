///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment/ISingleElementConnector.h.
///
/// NDE file domain path: probes[index] / conventionalRound or conventionalRectangular / elements[index] / connectorName
/// 
/// Declares the ISingleElementConnector interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/ISinglePulserReceiverPort.h>

namespace Olympus { namespace Equipment {

  class ISingleElementConnectorSplitter;
  using ISingleElementConnectorSplitterPtr = std::shared_ptr<ISingleElementConnectorSplitter>;

class ISingleElementConnector
{
public:
  virtual ~ISingleElementConnector() = default;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Sets a connection
  ///
  /// @param connector The connector.
  ///
  /// @returns True if it succeeds, false if it fails.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool SetConnection(ISinglePulserReceiverPortPtr connector) = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gets the connection
  ///
  /// @returns The connection.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISinglePulserReceiverPortPtr GetConnection() const = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// Puts single element connector splitter
  ///
  /// @returns An ISingleElementConnectorSplitterPtr.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISingleElementConnectorSplitterPtr
    PutSingleElementConnectorSplitter() = 0;
};

using ISingleElementConnectorPtr = std::shared_ptr<ISingleElementConnector>;
using ISingleElementConnectorConstPtr =
  std::shared_ptr<const ISingleElementConnector>;
}}
