////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/ISmoothingFilterCollection.h
///
/// Declares the ISmoothingFilterCollection interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// This interface regroups all the different supported smoothing filters. It should be noted 
  /// that the frequency associated with the different filters represents the frequency of the
  /// probes that this filter is best suited for. Therefore, when looking for a smoothing
  /// filter, one should try to find one that best matches the frequency of the probe used.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class ISmoothingFilterCollection
  {
  public:
    virtual ~ISmoothingFilterCollection() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the number of smoothing filters supported.
    ///
    /// @return
    /// The number of filters in this collection.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetCount() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the frequency associated with the specifid filter. This frequency represents the 
    /// probe's frequency that this filter is best suited for. The chosen filter can be assigned
    /// to a beam set by using IFilterSettings::SetSmoothingFilter.
    ///
    /// @param index The index of smoothing frequency to retreive.
    ///
    /// @return
    /// The smoothing filter frequency value, in MegaHertz.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual double GetSmoothingFilter(std::size_t index) const = 0;

  };

  using ISmoothingFilterCollectionPtr = std::shared_ptr<ISmoothingFilterCollection>;
  using ISmoothingFilterCollectionConstPtr = std::shared_ptr<const ISmoothingFilterCollection>;
}
