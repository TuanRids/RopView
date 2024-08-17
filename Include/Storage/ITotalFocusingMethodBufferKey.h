////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ITotalFocusingMethodBufferKey.h.
///
/// ITotalFocusingMethodBufferKey is used as a unique key to retrieve a buffer and his descriptor.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <memory>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ITotalFocusingMethodBufferKey
      {
      public:
        virtual ~ITotalFocusingMethodBufferKey() = default;

        virtual const std::wstring& GetName() const = 0;
        virtual std::size_t GetColumnQuantity() const = 0;
      };

      using ITotalFocusingMethodBufferKeyConstPtr = std::shared_ptr<const ITotalFocusingMethodBufferKey>;
    }
  }
}
