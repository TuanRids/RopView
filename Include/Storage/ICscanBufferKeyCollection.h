////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferKeyCollection.h.
///
/// Declares the ICscanBufferKeyCollection interface, collecting all the key to access CScan buffers and descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/ICscanBufferKey.h>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanBufferKeyCollection
      {
      public:
        virtual ~ICscanBufferKeyCollection() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the key count for the collection
        ///
        /// @returns  The count.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t GetCount() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets buffer key for a specific index
        ///
        /// @param  index Zero-based index of the key.
        ///
        /// @returns  The buffer key.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ICscanBufferKeyConstPtr GetBufferKey(size_t index) const = 0;
      };

      using ICscanBufferKeyCollectionConstPtr = std::shared_ptr<const ICscanBufferKeyCollection>;
    }
  }
}
