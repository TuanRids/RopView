////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanMergedBufferGateKey.h.
///
/// ICscanMergedBufferGateKey is used as a unique key to retrieve 
/// the ICscanBuffer and his ICscanBufferDescriptor of a gate
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <string>
#include <Storage/IBufferKey.h>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
    class ICscanMergedBufferGateKey : public ICscanBufferKeyEx
      {
      public:
        virtual ~ICscanMergedBufferGateKey() = default;

        virtual const std::wstring& GetGateName() const = 0;

        virtual size_t GetBeamCount() const = 0;

      };

      using ICscanMergedBufferGateKeyConstPtr = std::shared_ptr<const ICscanMergedBufferGateKey>;
    }
  }
}
