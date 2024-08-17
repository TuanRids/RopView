////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanBuffer.h.
///
/// Declares the ICscanBuffer interface
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <Storage/ICscanData.h>
#include <Storage/ICscanBufferDescriptor.h>
#include <Storage/IReadOnlyCscanBuffer.h>


namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanBuffer : public IReadOnlyCscanBuffer
      {
      public:
        virtual ~ICscanBuffer() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Writes the CScan data
        ///
        /// @param  scanCell    The column of the patch. The column is contant in the scan direction.
        /// @param  indexCell   The row of the patch. The row is contant in the index direction.
        /// @param  position    The Cscan position value.
        /// @param  amplitude   The Cscan amplitude value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void Write(size_t scanCell, size_t indexCell, float position, int amplitude) = 0;

        virtual void Write(size_t scanCell, size_t indexCell, float position, int amplitude, bool synced, bool saturated, bool detected) = 0;

        virtual void Write(size_t scanCell, size_t indexCell, float crossingTime, float amplitudeTime, int amplitude, bool synced, bool saturated, bool detected) = 0;

      };

      using ICscanBufferPtr = std::shared_ptr<ICscanBuffer>;
    }
  }
}
