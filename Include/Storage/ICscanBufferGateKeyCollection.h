////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferGateKeyCollection.h.
///
/// Declares the ICscanBufferGateKeyCollection interface, collecting all 
/// the key to access CScan buffers and descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/IBufferKey.h>
#include <Storage/ICscanBufferGateKey.h>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanBufferGateKeyCollection : public IBufferKey
      {
      public:
        virtual ~ICscanBufferGateKeyCollection() = default;

        virtual size_t GetGateCount() const = 0;

        virtual ICscanBufferGateKeyConstPtr GetGateKey(size_t index) const = 0;
      };

      using ICscanBufferGateKeyCollectionConstPtr = std::shared_ptr<const ICscanBufferGateKeyCollection>;
    }
  }
}
