////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferBeamKey.h.
///
/// ICscanBufferBeamKey is used as a unique key to retrieve 
/// the ICscanBuffer and ICscanBufferDescriptor of a beam
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/IBufferKey.h>
#include <Storage/ICscanBufferBeamGateKey.h>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
    class ICscanBufferBeamKey : public ICscanBufferKeyEx
      {
      public:
        virtual ~ICscanBufferBeamKey() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the beam index  
        ///
        /// @returns  The beam index.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t GetBeamIndex() const = 0;

        virtual size_t GetCount() const = 0;

        virtual ICscanBufferBeamGateKeyConstPtr GetBeamGateKey(size_t index) const = 0;
      };

      using ICscanBufferBeamKeyConstPtr = std::shared_ptr<const ICscanBufferBeamKey>;
    }
  }
}
