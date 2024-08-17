////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferBeamKeyCollection.h.
///
/// Declares the ICscanBufferBeamKeyCollection interface, collecting all 
/// the key to access CScan buffers and descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/IBufferKey.h>
#include <Storage/ICscanBufferBeamKey.h>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanBufferBeamKeyCollection : public ICscanBufferKeyEx
      {
      public:
        virtual ~ICscanBufferBeamKeyCollection() = default;

        virtual size_t GetCount() const = 0;
        virtual ICscanBufferBeamKeyConstPtr GetBeamKey(size_t index) const = 0;
      };

      using ICscanBufferBeamKeyCollectionConstPtr = std::shared_ptr<const ICscanBufferBeamKeyCollection>;
    }
  }
}
