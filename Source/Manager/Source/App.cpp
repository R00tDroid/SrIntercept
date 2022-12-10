#include "App.hpp"
#include <Windows.h>
#include "detours.h"
#include "Packets.hpp"

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
    logger.SetPrefix("[SRI Manager] ");

    char exePathString[MAX_PATH];
    GetModuleFileNameA(GetModuleHandleA(nullptr), exePathString, MAX_PATH);
    std::filesystem::path exePath(exePathString);
    binaryDirectory = exePath.parent_path();

    dllPath = binaryDirectory / "SrIntercept.dll";

    logger.Log("SrIntercept manager: %s", exePath.string().c_str());
    logger.Log("SrIntercept interceptor: %s", dllPath.string().c_str());

    wchar_t* commandLine = GetCommandLineW();
    int argc = 0;
    wchar_t** args = CommandLineToArgvW(commandLine, &argc);

    connection = new HostConnection("127.0.0.1", 5678);

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            logger.Log("Injecting from commandline argument");
            std::string arg = Convert(std::wstring(args[i]));
            std::filesystem::path executable(arg);
            if (std::filesystem::exists(executable))
            {
                StartDynamicInjection(executable);
            }
            else
            {
                logger.Log("Could not find injection target: %s", executable.string().c_str());
            }
        }
    }

    outputCamera = scCreateCamera(1920, 1080, 0);
}

bool SrInterceptManager::Update()
{
    if (outputCamera != nullptr)
    {
        scSendFrame(outputCamera, nullptr);
    }

    std::vector<HostConnectionStream*> clientStreams = connection->GetStreams();
    for (HostConnectionStream* client : clientStreams)
    {
        client->BeginWrite();
        client->Write(PT_Test);
        client->Write<float>(6.2f);
        client->EndWrite();
    }

    return true;
}

SrInterceptManager::~SrInterceptManager()
{
}

void SrInterceptManager::StartDynamicInjection(std::filesystem::path executable)
{
    //TODO Run on a separate thread

    logger.Log("Injecting into: %s", executable.string().c_str());

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;
    ZeroMemory(&processInfo, sizeof(processInfo));

    if (DetourCreateProcessWithDllExA(executable.string().c_str(), nullptr, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, nullptr, nullptr, &startupInfo, &processInfo, dllPath.string().c_str(), nullptr))
    {
        logger.Log("Injection successful");
    }
    else
    {
        logger.Log("Failed to inject");
    }

    ResumeThread(processInfo.hThread);

    //WaitForSingleObject(processInfo.hProcess, INFINITE);

    //CloseHandle(&startupInfo);
    //CloseHandle(&processInfo);

    //logger.Log("Injection target has stopped");
}

void SrInterceptManager::InstallWebcam()
{
    HMODULE softcamModule = GetModuleHandleA("softcam.dll");
    auto DllRegisterServer = GetProcAddress(softcamModule, "DllRegisterServer");
    HRESULT result = DllRegisterServer();
    if (FAILED(result))
    {
        logger.Log("Failed to call DllRegisterServer");
        MessageBoxA(nullptr, "Failed to call DllRegisterServer", "", MB_OK + MB_ICONERROR);
        exit(-1);
    }
}

void SrInterceptManager::UninstallWebcam()
{
    HMODULE softcamModule = GetModuleHandleA("softcam.dll");
    auto DllUnregisterServer = GetProcAddress(softcamModule, "DllUnregisterServer");
    HRESULT result = DllUnregisterServer();
    if (FAILED(result))
    {
        logger.Log("Failed to call DllUnregisterServer");
        MessageBoxA(nullptr, "Failed to call DllUnregisterServer", "", MB_OK + MB_ICONERROR);
        exit(-1);
    }
}
