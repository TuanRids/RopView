#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IAdjustedParameterCollection;

  class IGate
  {
  public:
    virtual ~IGate() = default;

    __declspec(deprecated("Deprecated. Use InCycleData.")) virtual void Enable(bool state) = 0;

    virtual bool GetAbsoluteMode() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetAbsoluteMode(bool enable) = 0;

    virtual NanoSecond GetLength() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetLength(NanoSecond length) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @return
    /// The Start of the gate relative to the absolute zero (to the pulse) expressed in nanoseconds. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual NanoSecond GetStart() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the Start of the gate relative to the absolute zero (to the pulse) expressed in nanoseconds. 
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetStart(NanoSecond start) = 0;

    virtual int GetThreshold() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetThreshold(int threshold) = 0;

    __declspec(deprecated("Deprecated. Use InCycleData.")) virtual bool IsEnabled() const = 0;

    virtual void InCycleData(bool state) = 0;
    virtual bool IsInCycleData() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the gate's name
    ///
    /// @return The name of this gate
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::wstring GetName() const = 0;
  };

  using IGatePtr = std::shared_ptr<IGate>;
  using IGateConstPtr = std::shared_ptr<const IGate>;
}
