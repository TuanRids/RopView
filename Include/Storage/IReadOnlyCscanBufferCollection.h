////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IReadOnlyCscanBufferCollection.h.
///
/// Declares the IReadOnlyCscanBufferCollection interface used to store the CScan of one beamset
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <Storage/IReadOnlyCscanBuffer.h>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class IReadOnlyCscanBufferCollection
      {
      public:
        virtual ~IReadOnlyCscanBufferCollection() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the ICScan buffer count, equal to the beam quantity of the beamset
        ///
        /// @returns  The IAScan buffer count.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t GetCount() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets a buffer
        ///
        /// @param  index Zero-based index of the ICscanbuffer.
        ///
        /// @returns  The buffer.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IReadOnlyCscanBufferConstPtr GetBuffer(size_t index) const = 0;
      };

      using IReadOnlyCscanBufferCollectionConstPtr = std::shared_ptr<const IReadOnlyCscanBufferCollection>;
    }
  }
}
