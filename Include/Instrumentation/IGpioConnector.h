#pragma once
#include <instrumentation/InstrumentationTypes.h>
#include <ultrasoundConfiguration/UltrasoundTypes.h>

namespace Instrumentation
{
  class IDigitalInputPinCollection;
  class IDigitalOutputPinCollection;

  class IGpioConnector
  {
  public:
    virtual ~IGpioConnector() = default;

    virtual const std::wstring& GetName() const = 0;

    virtual std::shared_ptr<IDigitalOutputPinCollection> GetDigitalOutputPinCollection() const = 0;

    virtual std::shared_ptr<IDigitalInputPinCollection> GetDigitalInputPinCollection() const = 0;
  };

  using IGpioConnectorPtr = std::shared_ptr<IGpioConnector>;
  using IGpioConnectorConstPtr = std::shared_ptr<const IGpioConnector>;
}
