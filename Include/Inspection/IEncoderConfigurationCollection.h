///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IEncoderConfigurationCollection.h.
///
/// NDE file domain path: motionDevices
/// 
/// Declares the IEncoderConfigurationCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Equipment/IEncoderSensor.h>
#include <Inspection/IEncoderConfiguration.h>

namespace Olympus
{
  namespace Inspection
  {
    class IEncoderConfigurationCollection
    {
    public:
      virtual ~IEncoderConfigurationCollection() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the EncoderSensor Configurations count
      ///
      /// @returns The count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets an encoder configuration
      ///
      /// @param index Zero-based index of the encoder.
      ///
      /// @returns The encoder configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IEncoderConfigurationPtr GetEncoderConfiguration(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets an encoder configuration
      ///
      /// @param index Zero-based index of the encoder.
      ///
      /// @returns The encoder configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IEncoderConfigurationConstPtr GetEncoderConfiguration(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds an encoder
      ///
      /// EncoderSensor object must have at least its phase A pin connected to a device.
      ///
      /// @param encoder The encoder to add.
      ///
      /// @returns An IEncoderConfigurationPtr.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IEncoderConfigurationPtr Add(Olympus::Equipment::IEncoderSensorPtr encoder) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the given encoder
      ///
      /// The indexes of the elements after the removed element are all decremented of 1.
      ///
      /// @param index The index of the encoder to remove.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Remove(std::size_t index) = 0;
    };

    /// Zero-based index of the encoder configuration collection pointer
    using IEncoderConfigurationCollectionPtr = std::shared_ptr<IEncoderConfigurationCollection>;
    using IEncoderConfigurationCollectionConstPtr = std::shared_ptr<const IEncoderConfigurationCollection>;
  }
}
