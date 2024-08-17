#pragma once

#if defined(STORAGE_EXPORTS)
#define STORAGE_API __declspec(dllexport)
#else
#define STORAGE_API __declspec(dllimport)
#pragma comment(lib, "OlympusNDT.Storage.lib")
#endif
