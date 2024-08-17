#pragma once
#include <memory>
#include <Inspection/InspectionEnums.h>

namespace Olympus
{
  namespace Inspection
  {
    enum class EventType;
    enum class AmplitudeType;

#ifndef SWIG
    constexpr wchar_t GATE_I[] = L"Gate I";
    constexpr wchar_t GATE_A[] = L"Gate A";
    constexpr wchar_t GATE_B[] = L"Gate B";
    constexpr wchar_t GATE_C[] = L"Gate C";
    constexpr wchar_t GATE_D[] = L"Gate D";
#endif


    class IGateConfigurationFunction;

    struct GateSynchroConfig
    {
      enum class GateSynchronizationMode { Absolute, RelativeGate };

      GateSynchronizationMode type;
      int gateIndex; // Only used when GateSynchronizationMode is RelativeGate.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Describes a Gate.
    ///
    /// Here are two simple scenarios to demonstrated the different Delays in the gates and their relation relative to other gates.
    ///
    ///
    /// @image html StorageCscsanWeldInspectionScenario.png "Weld Inspection to Demonstrate non Relative Gates" width=75%
    ///
    ///
    /// @image html StorageCscsanCompositeInspectionScenario.png "Weld Inspection to Demonstrate Relative Gates" width=75%
    ///
    ///
    /// @image html StorageCscsanGateSynchronizationErrors.png "Exceptions" width=75%
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class IGateConfiguration
    {
    public:
      virtual ~IGateConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets absolute mode
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / synchronization / mode ("Pulse", "GateRelative")
      /// 
      /// @returns True if it succeeds, false if it fails.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool GetAbsoluteMode() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets absolute mode
      ///
      /// @param enable True to enable, false to disable.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetAbsoluteMode(bool enable) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the length
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / length
      /// 
      /// @returns The length in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetLength() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a length
      ///
      /// @param length The length in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetLength(double length) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / start
      /// 
      /// @return
      /// The Delay of the gate begining relative to the surfaceZero (WedgeDelay + BeamDelay = surfaceZero) expressed in nanoseconds.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetDelay() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Set The Delay of the gate begining relative to the surfaceZero (WedgeDelay + BeamDelay =
      /// surfaceZero) expressed in nanoseconds.
      ///
      /// @param delay The delay.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetDelay(double delay) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the threshold
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / threshold
      /// 
      /// @returns The threshold in percent.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetThreshold() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a threshold
      ///
      /// @param threshold The threshold in percent.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetThreshold(int threshold) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if this Cscan data are produced by a device.
      ///
      /// @returns True if cscan data are from a device, false if not.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool InCycleData() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Set if Cscan data are produced by a device or not
      ///
      /// @param state True if Cscan data are from device.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void InCycleData(bool state) = 0;

      virtual std::shared_ptr<const IGateConfigurationFunction> GetFunction() const = 0;
      virtual void SetFunction(EventType eventType, AmplitudeType amplitudeType) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets synchro configuration
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / synchronization
      /// 
      /// @returns The synchro configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual GateSynchroConfig GetSynchroConfig() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets synchro configuration
      ///
      /// @param synchroConfig The synchro configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetSynchroConfig(GateSynchroConfig synchroConfig) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if this object is reserve cscan buffer
      ///
      /// @returns True if cscan buffer available, false if not.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsReserveCscanBuffer() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Reserve cscan buffer
      ///
      /// @param reserve True to reserve buffer.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void ReserveCscanBuffer(bool reserve) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the name.
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / name
      /// 
      /// @return
      /// The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the Cscan sampling resolution
      ///
      /// 
      /// 
      /// @returns The Cscan sampling resolution in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetCscanSamplingResolution() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the Cscan sampling resolution
      ///
      /// @param resolution The Cscan sampling resolution in nanoseconds.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetCscanSamplingResolution(double resolution) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the placement of the gate
      ///
      /// NDE file domain path sectorialPulseEcho / gates[index] / geometry
      /// 
      /// @returns The gate placement.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual GatePlacement GetPlacement() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the placement of the gate
      ///
      /// 
      /// @param geometry The gate placement.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPlacement(GatePlacement placement) = 0;
    };

    using IGateConfigurationPtr = std::shared_ptr<IGateConfiguration>;
    using IGateConfigurationConstPtr = std::shared_ptr<const IGateConfiguration>;
  }
}
