#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class ITcgPoint;
  class IAdjustedParameterCollection;

  class ITcgPointCollection
  {
  public:
    virtual ~ITcgPointCollection() = default;

    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<ITcgPoint> GetTcgPoint(std::size_t index) const = 0;

    __declspec(deprecated("Deprecated. Use ITCGPointCollection::AddTcgPointEx to get more verbosity on which automatic adjustments were applied")) virtual bool AddTcgPoint(NanoSecond position, double gain) = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Construct and add another point to the collection using this method.
    /// The new ITcgPoint to be added will first be validaded against other points in the TCG.
    /// If the point fits (i.e. respects the rules of the current TcgType), it will be inserted
    /// and adjusted. The actual index of the new point in the collection, which is sorted, is
    /// returned in the IAdjustedParameterCollection via the ObjectKey.
    /// If the point does not fit, then the returned index will be greater than the maximum
    /// number of points of the TCG (e.g. UINT32_MAX) and the point will not be added.
    ///
    /// @param position The desired position of the new point.
    /// @param gain The desired gain of the new point.
    ///
    /// @return
    /// The interface IAdjustedParameterCollection containing the parameters that were adjusted by
    /// the insertion. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAdjustedParameterCollection> AddTcgPointEx(NanoSecond position, double gain) = 0;
    virtual void Remove(std::size_t index) = 0;
    virtual void RemoveAll() = 0;
  };

  using ITcgPointCollectionPtr = std::shared_ptr<ITcgPointCollection>;
  using ITcgPointCollectionConstPtr = std::shared_ptr<const ITcgPointCollection>;
}
