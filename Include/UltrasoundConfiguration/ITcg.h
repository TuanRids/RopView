////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/ITCG.h
///
/// Declares the ITCG interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class ITcgPointCollection;
  class IAdjustedParameterCollection;

  class ITcg
  {
  public:
    enum class Type { None = -1, Digital, Hybrid };
    virtual ~ITcg() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the collection of TCG points associated with this beam. 
    /// 
    /// @return
    /// The ITcgPointCollection containing the list of TCG points.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<ITcgPointCollection> GetTcgPointCollection() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Change the type of TCG to use for this beam.
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that are impacted
    /// by this configuration. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> SetTcgType(Type tcgType) = 0;

    virtual Type GetTcgType() const = 0;
    virtual void Enable(bool doEnable) = 0;
    virtual bool IsEnabled() const = 0;
  };

  using ITcgPtr = std::shared_ptr<ITcg>;
  using ITcgConstPtr = std::shared_ptr<const ITcg>;
}
