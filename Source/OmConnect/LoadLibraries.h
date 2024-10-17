#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <filesystem>

#include <iostream>

using namespace std;
namespace fs = std::filesystem;

// Installation of OpenView SDK or OpenView has to be done previously.
// Add _PLATFORM_TOOLSET=$(PlatformToolsetVersion) to Preprocessor project settings.
// Set the /DelayLoad option for OlympusNDT.Instrumentation.dll and or
// OlympusNDT.Storage.dll.

namespace OpenView
{
    class Libraries
    {
    public:
        static bool IsBinariesBeside;

        static HINSTANCE LoadInstrumentation()
        {
            HINSTANCE handle(nullptr);
            const std::wstring fileName(L"OlympusNDT.Instrumentation.dll");

            fs::path fullyQualifiedFileName = GetFullyQualifiedFileName(fileName);
            const std::wstring libFileName = fullyQualifiedFileName;
            const std::wstring libPath = fullyQualifiedFileName.remove_filename();

            if (!IsBinariesBeside)
                SetDllDirectory(libPath.c_str());

            if ((handle = LoadLibrary(libFileName.c_str())) == nullptr)
                throw exception("Can not load Instrumentation library.");

            if (!IsBinariesBeside)
                SetDllDirectory(nullptr);

            return handle;
        }

        static HINSTANCE LoadStorage()
        {
            HINSTANCE handle(nullptr);
            const std::wstring fileName(L"OlympusNDT.Storage.dll");

            fs::path fullyQualifiedFileName = GetFullyQualifiedFileName(fileName);
            const std::wstring libFileName = fullyQualifiedFileName;
            const std::wstring libPath = fullyQualifiedFileName.remove_filename();

            if (!IsBinariesBeside)
                SetDllDirectory(libPath.c_str());

            if ((handle = LoadLibrary(libFileName.c_str())) == nullptr)
                throw exception("Can not load Storage library.");

            if (!IsBinariesBeside)
                SetDllDirectory(nullptr);

            return handle;
        }

        static void UnLoad(HINSTANCE handle)
        {
            if (FreeLibrary(handle) == 0)
                throw exception("Can not unload library.");
        }

        static fs::path GetFullyQualifiedFileName(const std::wstring& libName)
        {
            fs::path libFilePath;
            libFilePath = GetCurrentModuleFilePath() + L"\\" + libName;
            if (fs::exists(libFilePath)) {
                return libFilePath;
            }

            HKEY hKey;
            fs::path config;
            IsBinariesBeside = false;

            fs::path runtimeLibFilePath;
            wstring keyName = L"SOFTWARE\\OlympusNDT\\OpenView\\1.0\\";
            long result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyName.c_str(), 0, KEY_READ, &hKey);
            if (result == ERROR_SUCCESS)
            {
                wchar_t versionPath[1024];
                unsigned long versionPathSize = sizeof(versionPath);
                result = RegQueryValueExW(hKey, wstring(L"VersionPath").c_str(), 0, NULL, (LPBYTE)versionPath, &versionPathSize);
                if (result == ERROR_SUCCESS)
                {
                    runtimeLibFilePath = fs::path(versionPath) / LR"(Bin\)";

#if (_PLATFORM_TOOLSET == 140) // VS2015
                    runtimeLibFilePath += LR"(v140\)";
#elif (_PLATFORM_TOOLSET == 141) // VS2017
                    runtimeLibFilePath += LR"(v141\)";
#endif

                    runtimeLibFilePath += config;
                    runtimeLibFilePath += libName;

                    return runtimeLibFilePath;
                }
            }

            fs::path sdkLibFilePath;
            keyName = L"SOFTWARE\\OlympusNDT\\OpenView SDK\\1.0\\";
            result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyName.c_str(), 0, KEY_READ, &hKey);
            if (result == ERROR_SUCCESS)
            {
                wchar_t versionPath[1024];
                unsigned long versionPathSize = sizeof(versionPath);
                result = RegQueryValueExW(hKey, wstring(L"VersionPath").c_str(), 0, NULL, (LPBYTE)versionPath, &versionPathSize);
                if (result == ERROR_SUCCESS)
                {
                    sdkLibFilePath = fs::path(versionPath) / LR"(Bin\)";

#if (_PLATFORM_TOOLSET == 140) // VS2015
                    sdkLibFilePath += LR"(v140\)";
#elif (_PLATFORM_TOOLSET == 141) // VS2017
                    sdkLibFilePath += LR"(v141\)";
#endif

#ifdef _DEBUG
                    config = LR"(x64\Debug\)";
#else
                    config = LR"(x64\Release\)";
#endif

                    sdkLibFilePath += config;
                    sdkLibFilePath += libName;

                    return sdkLibFilePath;
                }
            }

            if (!fs::exists(runtimeLibFilePath) && !fs::exists(sdkLibFilePath))
                throw exception("Library file path not found.");

            return libFilePath;
        }

        static std::wstring GetCurrentModuleFilePath()
        {
            HMODULE thisModuleHandle = NULL;
            if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)&GetCurrentModuleFilePath, &thisModuleHandle))
            {
                throw std::runtime_error("Can not get the current module file path.");
            }

            wchar_t dllPath[MAX_PATH] = { 0 };
            GetModuleFileName(thisModuleHandle, dllPath, sizeof(dllPath));
            fs::path dllDirectory = fs::path(dllPath).remove_filename();

            return dllDirectory.wstring();
        }
    };

    bool OpenView::Libraries::IsBinariesBeside = true;
}
