#pragma once

namespace Instrumentation
{
  enum class DataUnit
  {
    Sample,
    Nanosecond,
    Percent,
    Decibel
  };

  class  IDataRange
  {
  public:
    virtual ~IDataRange() = default;

    __declspec(deprecated("Deprecated. Use IDataRange::GetFloatingMin to get better precision")) virtual int GetMin() const = 0;
    __declspec(deprecated("Deprecated. Use IDataRange::GetFloatingMax to get better precision")) virtual int GetMax() const = 0;

    virtual double GetFloatingMin() const = 0;
    virtual double GetFloatingMax() const = 0;
    virtual DataUnit GetUnit() const = 0;
  };

  using IDataRangePtr = std::shared_ptr<IDataRange>;
  using IDataRangeConstPtr = std::shared_ptr<const IDataRange>;
}
