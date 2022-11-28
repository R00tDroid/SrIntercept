#include "App.hpp"
#include <Windows.h>
#include "detours.h"

std::string Convert(std::wstring string)
{
    int stringLength = WideCharToMultiByte(CP_ACP, 0, string.c_str(), (int)string.length(), nullptr, 0, nullptr, nullptr);
    std::string str(stringLength, 0);
    WideCharToMultiByte(CP_ACP, 0, string.c_str(), -1, &str[0], stringLength, nullptr, nullptr);
    return str;
}

std::wstring Convert(std::string string)
{
    int stringLength = MultiByteToWideChar(CP_ACP, 0, string.c_str(), (int)string.length(), nullptr, 0);
    std::wstring wstr(stringLength, 0);
    MultiByteToWideChar(CP_ACP, 0, string.c_str(), (int)string.length(), &wstr[0], stringLength);
    return wstr;
}

SrInterceptManager::SrInterceptManager()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(GetModuleHandleA(nullptr), exePath, MAX_PATH);
    binaryDirectory = std::filesystem::path(exePath).parent_path();

    dllPath = binaryDirectory / "SrIntercept.dll";

    wchar_t* commandLine = GetCommandLineW();
    int argc = 0;
    wchar_t** args = CommandLineToArgvW(commandLine, &argc);

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg = Convert(std::wstring(args[i]));
            std::filesystem::path executable(arg);
            if (std::filesystem::exists(executable))
            {
                StartDynamicInjection(executable);
            }
            else
            {
                //TODO Report and log error
            }
        }
    }
}

bool SrInterceptManager::Update()
{
    return true;
}

SrInterceptManager::~SrInterceptManager()
{
}

void SrInterceptManager::StartDynamicInjection(std::filesystem::path executable)
{
    //TODO Run on a separate thread

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;
    ZeroMemory(&processInfo, sizeof(processInfo));

    if (DetourCreateProcessWithDllExA(executable.string().c_str(), nullptr, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, nullptr, nullptr, &startupInfo, &processInfo, dllPath.string().c_str(), nullptr))
    {
        //TODO Log success
    }
    else
    {
        //TODO Log failure
        MessageBoxA(nullptr, "Failed to inject", "", MB_OK + MB_ICONERROR);
    }

    ResumeThread(processInfo.hThread);

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    CloseHandle(&startupInfo);
    CloseHandle(&processInfo);
}