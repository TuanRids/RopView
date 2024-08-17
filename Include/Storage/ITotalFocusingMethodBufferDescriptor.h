////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ITotalFocusingMethodBufferDescriptor.h.
///
///  Contains all the definitions to interpret the buffer's data 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <memory>
#include <Storage/IDataAxis.h>
#include <Storage/ITotalFocusingMethodBufferKey.h>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class IDataAxis;
      using IDataAxisConstPtr = std::shared_ptr<const IDataAxis>;

      class ITotalFocusingMethodBufferDescriptor
      {
      public:
        virtual ~ITotalFocusingMethodBufferDescriptor() = default;

        virtual ITotalFocusingMethodBufferKeyConstPtr GetKey() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the scan axis description
        /// 
        /// @returns  The scan axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetScanAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the index axis description which also represent the focusing column quantity.
        /// 
        /// @returns  The index axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetIndexAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the Amplitude axis description in percent.
        /// 
        /// @returns  The amplitude axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetAmplitudeAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the ultrasound axis description to define the depth of the focusing in millimeter.
        /// 
        /// @returns  The ultrasound axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetUltrasoundAxis() const = 0;
      };

      using ITotalFocusingMethodBufferDescriptorConstPtr = std::shared_ptr<const ITotalFocusingMethodBufferDescriptor>;
    }
  }
}
