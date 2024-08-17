////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferKeyEx.h.
///
/// ICscanBufferKeyEx is used as a unique key to retrieve a buffer and his descriptor.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <memory>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanBufferKeyEx
      {
      public:
        virtual ~ICscanBufferKeyEx() = default;

        virtual const std::wstring& GetName() const = 0;
      };

      using ICscanBufferKeyExConstPtr = std::shared_ptr<const ICscanBufferKeyEx>;
    }
  }
}
