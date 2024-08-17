
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>


#ifdef MEMUTCONF_EXPORTS
#define MEMUTCONF_DECLSPEC __declspec(dllexport)
#else
#define MEMUTCONF_DECLSPEC __declspec(dllimport)
#endif


namespace Instrumentation
{
#ifdef SWIG
  enum UltrasoundTechnology { Conventional, PhasedArray };
#else
  enum class UltrasoundTechnology { Conventional, PhasedArray };
#endif

  typedef std::size_t FiringOrder;

  typedef double NanoSecond;
  typedef double Decibel;

}

#ifdef SWIGCSHARP
using namespace Instrumentation;
#endif
