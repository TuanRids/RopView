////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBufferDescriptor.h.
///
///  Contains all the definitions to interpret the buffer's data 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <memory>
#include <Storage/IDataAxis.h>
#include <Storage/ICscanBufferKey.h>
#include <Storage/IBufferKey.h>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanBufferDescriptor
      {
      public:
        virtual ~ICscanBufferDescriptor() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the key the get access to the CScanBuffer associated to this descriptor
        ///
        /// @returns  The key.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ICscanBufferKeyConstPtr GetKey() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the scan axis description
        /// 
        /// @returns  The scan axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetScanAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the index axis description
        /// 
        /// @returns  The index axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetIndexAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the Amplitude axis description. Typically the unit of this axis is "Percentage". This axis
        /// may be used in conjunction with the AmplitudeSamplingAxis to calculate a rule of three.
        /// 
        /// @returns  The amplitude axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetAmplitudeAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the Amplitude sampling axis description. Typically the unit of this axis is "Unit". This axis
        /// may be used in conjunction with the AmplitudeAxis to calculate a rule of three.
        /// 
        /// @returns  The amplitude sampling axis.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual IDataAxisConstPtr GetAmplitudeSamplingAxis() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Query if the buffer include every beams or not.
        ///
        /// @returns True if buffer will be merged, false if not.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool IsBufferMerged() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the key the get access to the buffer associated to this descriptor.
        /// Can be cast to ICscanBufferBeamGateKey or ICscanMergedBufferGateKey.
        ///
        /// @returns  The key.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ICscanBufferKeyExConstPtr GetKeyEx() const = 0;
      };

      using ICscanBufferDescriptorConstPtr = std::shared_ptr<const ICscanBufferDescriptor>;
    }
  }
}