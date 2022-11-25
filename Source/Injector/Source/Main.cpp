#include <windows.h>
#include "detours.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR Command, int)
{
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;
    ZeroMemory(&processInfo, sizeof(processInfo));

    if (DetourCreateProcessWithDllExA(Command, nullptr, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, nullptr, nullptr, &startupInfo, &processInfo, "Intercept.dll", nullptr))
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

    return EXIT_SUCCESS;
}
