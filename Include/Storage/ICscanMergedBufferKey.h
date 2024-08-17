////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanMergedBufferKey.h.
///
/// ICscanMergedBufferKey is used as a unique key to retrieve 
/// the ICscanBuffer and his ICscanBufferDescriptor
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/IBufferKey.h>
#include <Storage/ICscanMergedBufferGateKey.h>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanMergedBufferKey : public ICscanBufferKeyEx
      {
      public:
        virtual ~ICscanMergedBufferKey() = default;

        virtual size_t GetBeamCount() const = 0;

        virtual size_t GetGateCount() const = 0;

        virtual ICscanMergedBufferGateKeyConstPtr GetGateKey(size_t index) const = 0;
      };

      using ICscanMergedBufferKeyConstPtr = std::shared_ptr<const ICscanMergedBufferKey>;
    }
  }
}
