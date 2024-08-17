#pragma once

namespace Instrumentation
{
  enum
#ifndef SWIG
  class 
#endif
  DigitalPinState
  {
    Low, High,
  };
}

#ifdef SWIG
using namespace Instrumentation;
#endif
