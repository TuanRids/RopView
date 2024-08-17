////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IFiringBeamSetCollection.h
///
/// Declares the IFiringBeamSetCollection interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IBeamSet;
  class IConnector;
  class IFiringBeamSet;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Allows the addition and the removal of beam sets to be fired on conventional or phased array  
  /// connector(s) by using their respective interface. 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IFiringBeamSetCollection
  {
  public:
    virtual ~IFiringBeamSetCollection() = default;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Add a beam set to be fired to the connectors used in a configuration by providing the
    /// interfaces of the connectors and the pins to use on these connectors.
    /// 
    /// The firing order of the added beams will be sequential and will start after the highest firing
    /// order currently used by a beam
    ///
    /// @param beamSet       The interface IBeamSet of the beam set to be fired.
    /// @param connector     The interface IConnector of the connector used as the pulser.
    /// @param pulserPinId   (Optional) The number of the first pin to be used as pulser.
    /// @param receiverPinId (Optional) The interface IConnector of the connector used as the receiver.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Add(std::shared_ptr<IBeamSet> beamSet, std::shared_ptr<IConnector> connector, std::size_t pulserPinId = 1, std::size_t receiverPinId = 1) = 0;
    virtual void Add(std::shared_ptr<IBeamSet> beamSet, std::shared_ptr<IConnector> pulserConnector, std::shared_ptr<IConnector> receiverConnector, std::size_t pulserPinId = 1, std::size_t receiverPinId = 1) = 0;

    //////////////////////////////////////////////////////////////////////////
    /// Remove a fired beam set
    ///
    /// After removing a beam set, the firing order of all the remaining fired beams
    /// will be reset to their original order of creation
    ///
    /// The firing order sequence is guaranteed to be valid after the call
    ///
    /// @param beamSet The interface IBeamSet of the beam set desired to be removed.
    /// 
    /// - Throw if the beamSet is not fired
    //////////////////////////////////////////////////////////////////////////
    virtual void Remove(std::shared_ptr<IBeamSet> beamSet) = 0;

    virtual std::size_t GetCount() const = 0;

    virtual std::shared_ptr<IFiringBeamSet> GetFiringBeamSet(std::size_t index) const = 0;

    virtual std::shared_ptr<IFiringBeamSet> FindFiringBeamSet(std::wstring const &beamSetname) const = 0;

  };

  using IFiringBeamSetCollectionPtr = std::shared_ptr<IFiringBeamSetCollection>;
  using IFiringBeamSetCollectionConstPtr = std::shared_ptr<const IFiringBeamSetCollection>;
}
