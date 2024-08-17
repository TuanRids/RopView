////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IProbeConventional.h.
///
/// NDE file domain path probes[index] / conventionalRound
/// 
/// Declares the IProbeConventional interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <Equipment/IProbe.h>

namespace Olympus
{
  namespace Equipment
  {
    class ISingleElementConnector;
    using ISingleElementConnectorPtr = std::shared_ptr<ISingleElementConnector>;
    using ISingleElementConnectorConstPtr = std::shared_ptr<const ISingleElementConnector>;

    class ITransducer;
    using ITransducerPtr = std::shared_ptr<ITransducer>;
    using ITransducerConstPtr = std::shared_ptr<const ITransducer>;

    class IProbeConventional;
    using IProbeConventionalPtr = std::shared_ptr<IProbeConventional>;
    using IProbeConventionalConstPtr = std::shared_ptr<const IProbeConventional>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Conventional ultrasonic probe composed of a single element
    /// The information of this section is important to allow RayTracing inside the wedge.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class IProbeConventional : public IProbe
    {
    public:

      virtual ~IProbeConventional() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the transducer.
      ///
      /// @return
      /// The transducer.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual ITransducerPtr GetTransducer() = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the transducer.
      ///
      /// NDE file domain path probes[index] / conventionalRound
      /// 
      /// @return
      /// The transducer.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual ITransducerConstPtr GetTransducer() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a transducer.
      ///
      /// @param transducer The transducer.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetTransducer(ITransducerPtr transducer) = 0;

      virtual ISingleElementConnectorConstPtr GetConnector() const = 0;
      virtual ISingleElementConnectorPtr GetConnector() = 0;
    };
  }
}
