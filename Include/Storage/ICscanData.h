////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICscanData.h.
///
/// Access to the basic data of a CScan buffer
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus {
  namespace Ultrasound {
    namespace Data
    {
      class ICscanData
      {
      public:
        virtual ~ICscanData() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the time in the gate according to the event type.
        /// In nanoseconds, calculate from the gate start
        /// Only one time is supported when reading .fpd files, 
        /// GetCrossingTime and GetAmplitudeTime can not be used with .fpd files.
        /// 
        /// @image html StorageCscsanWeldInspectionScenario.png "Cscan and Gates" width=50%
        ///
        /// @returns  The position.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual float GetPosition() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the amplitude of the event, in sampling units. Typically 0 to 255 for 8 bits acquisition
        /// and 0 to 2047 for 12 bits. The axis to ICscanBufferDescriptor::GetAmplitudeSamplingAxis provides
        /// the minimum and maximum values.
        ///
        /// @returns  The amplitude.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int GetAmplitude() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Query if data has been written.
        ///
        /// @returns  True is data is available.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasData() const = 0;
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Query if data are saturated.
        ///
        /// @returns  True if data are saturated.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool IsSaturated() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Query if data are synchronized with the source.
        /// Always synced when the source is the main bang.
        /// Source can be any previous gates.
        ///
        /// @returns  True if data are synced.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool IsSyncedOnSource() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Query if data exceeds the gate threshold.
        ///
        /// @returns  True if data are higher than the threshold.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool IsDetected() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the crossing time in the gate, in nanoseconds, calculate from the gate start
        /// Not supported with .fpd files, must use GetPostion instead.
        /// 
        /// @image html StorageCscsanWeldInspectionScenario.png "Cscan and Gates" width=50%
        ///
        /// @returns  The time.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual float GetCrossingTime() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the time of the amplitude according to the amplitude type in the IGateConfigurationFunction,
        /// In nanoseconds, calculate from the gate start
        /// Not supported with .fpd files, must use GetPostion instead.
        /// 
        /// @image html StorageCscsanWeldInspectionScenario.png "Cscan and Gates" width=50%
        ///
        /// @returns  The time.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual float GetAmplitudeTime() const = 0;
      };

      using ICscanDataConstPtr = std::shared_ptr<const ICscanData>;
    }
  }
}
