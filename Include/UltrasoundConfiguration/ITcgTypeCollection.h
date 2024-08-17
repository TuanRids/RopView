////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IDigitalBandPassFilterCollection.h
///
/// Declares the IDigitalBandPassFilterCollection interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include <ultrasoundConfiguration/ITcgType.h>

namespace Instrumentation
{
  class ITcgTypeCollection
  {
  public:
    virtual ~ITcgTypeCollection() = default;

    virtual std::size_t GetCount() const = 0;

    virtual std::shared_ptr<ITcgType> GetTcgType(std::size_t index) const = 0;

  };
  
  using ITcgTypeCollectionPtr = std::shared_ptr<ITcgTypeCollection>;
  using ITcgTypeCollectionConstPtr = std::shared_ptr<const ITcgTypeCollection>;
}
