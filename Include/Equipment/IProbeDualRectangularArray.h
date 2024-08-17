///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Equipment\IProbeDualRectangularArray.h.
///
/// NDE file domain path probes[index] / PhasedArrayLinear
/// 
/// Declares the IProbeDualRectangularArray interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IProbe.h>

namespace Olympus {  namespace Equipment {

  class IMultiElementConnector;
  using IMultiElementConnectorPtr = std::shared_ptr<IMultiElementConnector>;
  using IMultiElementConnectorConstPtr = std::shared_ptr<const IMultiElementConnector>;

  class IMultiElementConnectorCollection;
  using IMultiElementConnectorCollectionPtr = std::shared_ptr<IMultiElementConnectorCollection>;
  using IMultiElementConnectorCollectionConstPtr = std::shared_ptr<const IMultiElementConnectorCollection>;

  class ITransducerRectangularArray;
  using ITransducerRectangularArrayPtr = std::shared_ptr<ITransducerRectangularArray>;
  using ITransducerRectangularArrayConstPtr = std::shared_ptr<const ITransducerRectangularArray>;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Phased-array ultrasonic probe including 2 transducers 2D array of elements
  /// The information of this section is important to allow RayTracing inside the wedge.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IProbeDualRectangularArray : public IProbe
  {
  public:

    virtual ~IProbeDualRectangularArray() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the pulsing transducer pointer.
    ///
    /// NDE file domain path probes[indexPulsing] / PhasedArrayLinear
    /// 
    /// @return The pulsing transducer pointer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerRectangularArrayPtr GetPulsingTransducer() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the pulsing transducer constant pointer.
    ///
    /// NDE file domain path probes[indexPulsing] / PhasedArrayLinear
    /// 
    /// @return The pulsing transducer constant pointer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerRectangularArrayConstPtr GetPulsingTransducer() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a pulsing transducer.
    ///
    /// @param pulsingTransducer The pulsing transducer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetPulsingTransducer(ITransducerRectangularArrayPtr pulsingTransducer) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the receiving transducer pointer.
    ///
    /// NDE file domain path probes[indexReceiving] / PhasedArrayLinear
    /// 
    /// @return The receiving transducer pointer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerRectangularArrayPtr GetReceivingTransducer() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the receiving transducer constant pointer.
    ///
    /// NDE file domain path probes[indexReceiving] / PhasedArrayLinear
    /// 
    /// @return The receiving transducer constant pointer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerRectangularArrayConstPtr GetReceivingTransducer() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a receiving transducer.
    ///
    /// @param receivingTransducer The receiving transducer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetReceivingTransducer(ITransducerRectangularArrayPtr receivingTransducer) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the mounting orientation of the pulsing transducer.
    ///
    /// NDE file domain path probes[indexPulsing] / wedgeAssociation / orientation  ["Normal", "Reverse"]
    /// 
    /// @return The mounting orientation of the pulsing transducer is either Normal (0 degrees),
    /// Lateral (90 degrees), ReverseNormal (180 degrees) or ReverseLateral (270 degrees).
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual MountingOrientation GetMountingOrientationPulsingTransducer() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a mounting orientation for the pulsing transducer.
    ///
    /// @param orientation The orientation for the pulsing transducer is either Normal (0 degrees),
    /// Lateral (90 degrees), ReverseNormal (180 degrees) or ReverseLateral (270 degrees).
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetMountingOrientationPulsingTransducer(MountingOrientation orientation) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the mounting orientation of the receiving transducer.
    ///
    /// NDE file domain path probes[indexReceiving] / wedgeAssociation / orientation  ["Normal", "Reverse"]
    /// 
    /// @return The mounting orientation of the receiving transducer is either Normal (0 degrees),
    /// Lateral (90 degrees), ReverseNormal (180 degrees) or ReverseLateral (270 degrees).
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual MountingOrientation GetMountingOrientationReceivingTransducer() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets a mounting orientation for the receiving transducer.
    ///
    /// @param orientation The orientation for the receiving transducer is either Normal (0 degrees),
    /// Lateral (90 degrees), ReverseNormal (180 degrees) or ReverseLateral (270 degrees).
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetMountingOrientationReceivingTransducer(MountingOrientation orientation) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the connector
    ///
    /// @returns The connector.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IMultiElementConnectorConstPtr GetConnector() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the connector
    ///
    /// @returns The connector.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IMultiElementConnectorPtr GetConnector() = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the connectors
    ///
    /// @returns The connectors.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IMultiElementConnectorCollectionConstPtr
      GetConnectors() const = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the connectors
    ///
    /// @returns The connectors.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IMultiElementConnectorCollectionPtr GetConnectors() = 0;
  };

  using IProbeDualRectangularArrayPtr = std::shared_ptr<IProbeDualRectangularArray>;
  using IProbeDualRectangularArrayConstPtr = std::shared_ptr<const IProbeDualRectangularArray>;
}}
