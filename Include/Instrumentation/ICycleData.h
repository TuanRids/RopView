////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICycleData.h
///
/// Declares the ICycleData interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IAscanCollection;
  class IEncoderDataCollection;
  class IDigitalInputDataCollection;
  class ICscanCollection;

  using CycleId = std::uint64_t;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Contains all the information of a data cycle. During a single data cycle, all beams 
  /// in all beam sets on all the devices produce an AScan.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class ICycleData
  {
  public:
    virtual ~ICycleData() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the identification number of the data cycle.
    ///
    /// @return
    /// The Cycle Id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CycleId GetCycleId() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the collection of Ascan produced during this data cycle.
    ///
    /// @return
    /// The interface for the collection of Ascan.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IAscanCollection> GetAscanCollection() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //TODO: document ICycleData::GetEncoderDataCollection
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<IEncoderDataCollection> GetEncoderDataCollection() const = 0;

    virtual std::shared_ptr<IDigitalInputDataCollection> GetDigitalInputDataCollection() const = 0;

    virtual std::shared_ptr<ICscanCollection> GetCscanCollection() const = 0;
  };
}