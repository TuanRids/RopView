///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IDigitalInputPinConfigurationCollection.h.
///
/// NDE file domain path Not Available in NDE file format
/// 
/// Declares the IDigitalInputPinConfigurationCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "IDigitalInputPinConfiguration.h"

namespace Olympus
{
  namespace Inspection
  {
    class IDigitalInputPinConfigurationCollection
    {
    public:
      virtual ~IDigitalInputPinConfigurationCollection() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the count for the number of digital input pin configurations in this collection.
      ///
      /// @return
      /// The number of digital input pin configurations.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual size_t GetCount() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the interface of a digital input pin configuration in this set by giving its index in the collection.
      ///
      /// @param index The index of the requested digital input pin configuration in this collection.
      ///
      /// @return
      /// The interface IDigitalInputPinConfiguration of the requested digital input pin configuration.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IDigitalInputPinConfigurationPtr GetPinConfiguration(size_t index) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Get the interface of a digital input pin configuration in this set by giving its signal.
      ///
      /// @param signal The signal of the requested digital input pin configuration in this collection.
      ///
      /// @return
      /// The interface IDigitalInputPinConfiguration of the requested digital input pin configuration.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IDigitalInputPinConfigurationPtr FindPinConfiguration(Equipment::PinSignal signal) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a digital input pin configuration to this set
      ///
      /// Only one configuration can exist in the collection for a given signal
      ///
      /// @param signal The pin signal.
      /// @param action The action of the pin.
      /// @param connectorName The name of the pin's connector
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IDigitalInputPinConfigurationPtr Add(Equipment::PinSignal signal, Instrumentation::IDigitalInputPin::Action action, std::wstring connectorName) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Remove a digital input pin configuration from this set by giving its index in the collection.
      ///
      /// @param index The index of the digital input pin configuration to remove from this collection.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void RemoveByIndex(size_t index) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Remove a digital input pin configuration from this set by giving its signal.
      ///
      /// @param signal The signal of the digital input pin configuration to remove from this collection.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void RemoveBySignal(Equipment::PinSignal signal) = 0;
    };

    using IDigitalInputPinConfigurationCollectionPtr = std::shared_ptr<IDigitalInputPinConfigurationCollection>;
    using IDigitalInputPinConfigurationCollectionConstPtr = std::shared_ptr<const IDigitalInputPinConfigurationCollection>;
  }
}


