////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ultrasoundConfiguration/IElementDelayCollection.h
///
/// Declares the IDelayCollection interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IElementDelay;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// This interface contains the delays of a pulser or receiver for a beam formation. 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IElementDelayCollection
  {
  public:
    virtual ~IElementDelayCollection() = default;

    virtual std::size_t GetCount() const = 0;
    virtual std::shared_ptr<IElementDelay> GetElementDelay(std::size_t index) const = 0;
  };

  using IElementDelayCollectionPtr = std::shared_ptr<IElementDelayCollection>;
  using IElementDelayCollectionConstPtr = std::shared_ptr<const IElementDelayCollection>;
}
