///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Indication\IIndicationCollection.h.
///
/// NDE file domain path / The custom section are not part of the Domain,
/// 
/// Declares the IIndicationCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <Indication/IIndication.h>

namespace Olympus
{
  namespace Indication
  {
    /// Collection of indications.
    class IIndicationCollection
    {
    public:
      virtual ~IIndicationCollection() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the number of indications in the collection.
      ///
      /// @returns The count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets an indication.
      ///
      /// @param index The index of the indication.
      ///
      /// @returns A constant pointer to the indication.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IIndicationConstPtr GetIndication(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets an indication
      ///
      /// @param index The index of the indication.
      ///
      /// @returns A pointer to the indication.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IIndicationPtr GetIndication(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds an indication to the collection.
      ///
      /// @param beamsetName Name of the beamset.
      /// @param beamIndex   Zero-based index of the beam.
      ///
      /// @returns A pointer tot the indication.
      /////////////////////////////////////////////////////////////////////////////////////////////
      __declspec(deprecated("Deprecated. Use AddEx")) virtual IIndicationPtr Add(const std::wstring& beamsetName, int beamIndex) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the indication with a given index.
      ///
      /// @param index The index of the indication to remove.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Remove(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds an indication to the collection.
      ///
      /// @param name The Name of the inspection method included in a configuration.
      /// @param index   Zero-based index of the beam or the colum.
      ///
      /// @returns A pointer tot the indication.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IIndicationPtr AddEx(const std::wstring& name, size_t index) = 0;
    };

    using IIndicationCollectionPtr = std::shared_ptr<IIndicationCollection>;
    using IIndicationCollectionConstPtr = std::shared_ptr<const IIndicationCollection>;
  }
}
