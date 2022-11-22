#include <windows.h>
#include "detours.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;
    ZeroMemory(&processInfo, sizeof(processInfo));

    if (DetourCreateProcessWithDllExA("TestApp.exe", NULL, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInfo, "Intercept.dll", NULL))
    {
        MessageBoxA(0, "Injected successfully", "", MB_OK + MB_ICONINFORMATION);
    }
    else
    {
        MessageBoxA(0, "Failed to inject", "", MB_OK + MB_ICONERROR);
    }

    ResumeThread(processInfo.hThread);

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    CloseHandle(&startupInfo);
    CloseHandle(&processInfo);

    return EXIT_SUCCESS;
}
