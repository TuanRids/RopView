////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IReadOnlyAscanBufferCollection.h.
///
/// Declares the IReadOnlyAscanBufferCollection interface used to store one beamset
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/IReadOnlyAscanBuffer.h>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class IReadOnlyAscanBufferCollection
      {
      public:
        virtual ~IReadOnlyAscanBufferCollection() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the IAScan buffer count, equal to the beam quantity of the beamset
        ///
        /// @returns  The IAScan buffer count.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t GetCount() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets a buffer
        ///
        /// @param  index Zero-based index of the IAscanbuffer.
        ///
        /// @returns  The buffer.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IReadOnlyAscanBufferConstPtr GetBuffer(size_t index) const = 0;
      };

      using IReadOnlyAscanBufferCollectionConstPtr = std::shared_ptr<const IReadOnlyAscanBufferCollection>;
    }
  }
}
