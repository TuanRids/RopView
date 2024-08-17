#pragma once

#if defined(INSTRUMENTATION_EXPORTS)
#define INSTRUMENTATION_API __declspec(dllexport)
#else
#define INSTRUMENTATION_API __declspec(dllimport)
#pragma comment(lib, "OlympusNDT.Instrumentation.lib")
#endif
