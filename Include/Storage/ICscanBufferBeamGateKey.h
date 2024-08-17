////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferBeamGateKey.h.
///
/// ICscanBufferBeamGateKey is used as a unique key to retrieve 
/// the ICscanBuffer his ICscanBufferDescriptor of a beam gate combinaison.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Storage/IBufferKey.h>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
    class ICscanBufferBeamGateKey : public ICscanBufferKeyEx
      {
      public:
        virtual ~ICscanBufferBeamGateKey() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the beam index  
        ///
        /// @returns  The beam index.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t GetBeamIndex() const = 0;

        virtual const std::wstring& GetGateName() const = 0;
      };

      using ICscanBufferBeamGateKeyConstPtr = std::shared_ptr<const ICscanBufferBeamGateKey>;
    }
  }
}
